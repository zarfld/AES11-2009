#!/usr/bin/env python3
"""
Phase 05 quality metrics enforcement.

Enforces cyclomatic complexity, function length, parameter count, and duplication rate
using lizard. Thresholds are read from quality-gates.yml if present, otherwise from
environment variables, otherwise defaults per Phase 05 instructions.

Outputs a consolidated JSON report at build/quality-metrics.json and exits non-zero on breach.

Notes:
- Requires: lizard (pip install lizard), pyyaml (optional, only if quality-gates.yml present)
- Hardware/vendor-agnostic: operates on source text only.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import sys
from pathlib import Path

try:
    import yaml  # type: ignore
except Exception:  # pragma: no cover - optional dependency
    yaml = None  # Will fall back to env/defaults if not available


DEFAULTS = {
    "coverage.minimum": 80,
    "complexity.maximum": int(os.getenv("MAX_CYCLOMATIC_COMPLEXITY", "10")),
    "method_length.maximum": 50,
    "parameters.maximum": 4,
    "duplications.maximum_percent": 3.0,
}


def load_thresholds(config_path: Path | None) -> dict:
    cfg = {}
    # Load from YAML if available
    if config_path and config_path.exists() and yaml is not None:
        with config_path.open("r", encoding="utf-8") as f:
            try:
                raw = yaml.safe_load(f) or {}
            except Exception:
                raw = {}
        # Normalize keys into dot form
        if isinstance(raw, dict):
            if "coverage" in raw and isinstance(raw["coverage"], dict):
                cfg["coverage.minimum"] = raw["coverage"].get("minimum")
            if "complexity" in raw and isinstance(raw["complexity"], dict):
                cfg["complexity.maximum"] = raw["complexity"].get("maximum")
            if "method_length" in raw and isinstance(raw["method_length"], dict):
                cfg["method_length.maximum"] = raw["method_length"].get("maximum")
            if "parameters" in raw and isinstance(raw["parameters"], dict):
                cfg["parameters.maximum"] = raw["parameters"].get("maximum")
            if "duplications" in raw and isinstance(raw["duplications"], dict):
                # Accept both maximum and maximum_percent
                dup = raw["duplications"].get("maximum_percent") or raw["duplications"].get("maximum")
                cfg["duplications.maximum_percent"] = dup

    # Fill defaults where missing
    for k, v in DEFAULTS.items():
        if cfg.get(k) is None:
            cfg[k] = v
    # Coerce types safely (fallback to defaults if conversion fails)
    def safe_int(key: str) -> int:
        try:
            return int(cfg.get(key, DEFAULTS[key]))
        except Exception:
            return int(DEFAULTS[key])

    def safe_float(key: str) -> float:
        try:
            return float(cfg.get(key, DEFAULTS[key]))
        except Exception:
            return float(DEFAULTS[key])

    cfg["coverage.minimum"] = safe_int("coverage.minimum")
    cfg["complexity.maximum"] = safe_int("complexity.maximum")
    cfg["method_length.maximum"] = safe_int("method_length.maximum")
    cfg["parameters.maximum"] = safe_int("parameters.maximum")
    cfg["duplications.maximum_percent"] = safe_float("duplications.maximum_percent")
    return cfg


def run_lizard_json(paths: list[str]) -> dict:
    cmd = [
        "lizard",
        "-l",
        "cpp",
        "-j",
    ] + paths
    try:
        out = subprocess.check_output(cmd, text=True)
    except subprocess.CalledProcessError as e:
        print("Failed to run lizard for complexity JSON", file=sys.stderr)
        print(e.output, file=sys.stderr)
        raise
    try:
        return json.loads(out)
    except json.JSONDecodeError:
        # lizard sometimes prints warnings; attempt to strip lines before '{'
        start = out.find("{")
        return json.loads(out[start:])


def run_lizard_duplicate(paths: list[str]) -> tuple[float, str]:
    # Use duplicate extension; parse total duplicated rate line
    cmd = [
        "lizard",
        "-E",
        "duplicate",
    ] + paths
    try:
        out = subprocess.check_output(cmd, text=True, stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        out = e.output
    # Look for a line like: "Total duplicated rate: 2.3%"
    rate = 0.0
    m = re.search(r"Total duplicated rate:\s*([0-9]+(?:\.[0-9]+)?)%", out)
    if m:
        rate = float(m.group(1))
    return rate, out


def coerce_int(value, default: int) -> int:
    try:
        return int(value)
    except Exception:
        return default


def main() -> int:
    parser = argparse.ArgumentParser(description="Enforce code quality metrics using lizard")
    parser.add_argument("paths", nargs="*", default=["lib", "tests"], help="Paths to analyze")
    parser.add_argument("--config", default="quality-gates.yml", help="Path to thresholds config YAML")
    parser.add_argument("--out", default="build/quality-metrics.json", help="Output JSON path")
    args = parser.parse_args()

    thresholds = load_thresholds(Path(args.config) if args.config else None)

    # Ensure build dir
    out_path = Path(args.out)
    out_path.parent.mkdir(parents=True, exist_ok=True)

    # Complexity JSON
    lizard_json = run_lizard_json(args.paths)
    functions = []
    total_functions = 0
    # lizard reports a list of files each with function_list
    for file_entry in lizard_json.get("files", []):
        for fn in file_entry.get("functions", []) or file_entry.get("function_list", []):
            total_functions += 1
            functions.append({
                "name": fn.get("name"),
                "filename": fn.get("filename", file_entry.get("filename")),
                "nloc": coerce_int(fn.get("nloc"), 0),
                "ccn": coerce_int(fn.get("cyclomatic_complexity") or fn.get("ccn"), 0),
                "parameters": coerce_int(fn.get("parameter_count") or fn.get("parameters"), 0),
                "start_line": coerce_int(fn.get("start_line"), 0),
            })

    # Calculate offenders
    max_ccn = max([f["ccn"] for f in functions], default=0)
    max_nloc = max([f["nloc"] for f in functions], default=0)
    max_params = max([f["parameters"] for f in functions], default=0)

    complexity_offenders = [f for f in functions if f["ccn"] > thresholds["complexity.maximum"]]
    length_offenders = [f for f in functions if f["nloc"] > thresholds["method_length.maximum"]]
    param_offenders = [f for f in functions if f["parameters"] > thresholds["parameters.maximum"]]

    # Duplication
    dup_rate, dup_raw = run_lizard_duplicate(args.paths)

    status = {
        "complexity": max_ccn <= thresholds["complexity.maximum"],
        "method_length": max_nloc <= thresholds["method_length.maximum"],
        "parameters": max_params <= thresholds["parameters.maximum"],
        "duplications": dup_rate <= thresholds["duplications.maximum_percent"],
    }

    report = {
        "thresholds": thresholds,
        "summary": {
            "total_functions": total_functions,
            "max_ccn": max_ccn,
            "max_nloc": max_nloc,
            "max_parameters": max_params,
            "duplication_rate_percent": round(dup_rate, 2),
            "status": status,
        },
        "offenders": {
            "complexity": sorted(complexity_offenders, key=lambda x: x["ccn"], reverse=True)[:25],
            "method_length": sorted(length_offenders, key=lambda x: x["nloc"], reverse=True)[:25],
            "parameters": sorted(param_offenders, key=lambda x: x["parameters"], reverse=True)[:25],
        },
    }

    out_path.write_text(json.dumps(report, indent=2), encoding="utf-8")

    # Also persist raw duplicate output for debugging
    dup_out_path = out_path.parent / "duplication-report.txt"
    dup_out_path.write_text(dup_raw, encoding="utf-8")

    # Determine exit code
    failures = [k for k, ok in status.items() if not ok]
    if failures:
        print("❌ Quality gate(s) breached:", ", ".join(failures))
        print(f"  See {out_path} for details.")
        return 1
    print("✅ Quality metrics within thresholds.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
