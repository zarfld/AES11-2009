#!/usr/bin/env python3
"""Compute Defect Discovery Rate (DDR) and emit metrics JSON.

DDR = total_defects / KLOC (thousand lines of code) for sources under lib/.

Reads:
  docs/reliability/defect-log.yaml
Outputs:
  build/defect-metrics.json

Optional env:
  DDR_MAX -> if set (float), fail if DDR > DDR_MAX

Non-blocking if no defect log or empty list.
"""
import os
import sys
import json
import pathlib
from typing import Any, Dict

try:
    import yaml  # type: ignore
except ImportError:
    print("yaml module not found. Install with: pip install pyyaml", file=sys.stderr)
    sys.exit(1)

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
DEFECT_LOG = ROOT / "docs" / "reliability" / "defect-log.yaml"
OUTPUT_DIR = ROOT / "build"
OUTPUT_FILE = OUTPUT_DIR / "defect-metrics.json"
SOURCE_ROOT = ROOT / "lib"

def count_loc(path: pathlib.Path) -> int:
    loc = 0
    for p in path.rglob("*"):
        if p.suffix.lower() in {".c", ".cc", ".cpp", ".cxx", ".h", ".hpp", ".hxx"}:
            try:
                with p.open("r", encoding="utf-8", errors="ignore") as f:
                    for line in f:
                        stripped = line.strip()
                        if stripped and not stripped.startswith("//") and not stripped.startswith("/*"):
                            loc += 1
            except Exception as e:  # pragma: no cover
                print(f"Warning: failed reading {p}: {e}")
    return loc

def load_defects() -> Dict[str, Any]:
    if not DEFECT_LOG.exists():
        return {"items": []}
    with DEFECT_LOG.open("r", encoding="utf-8") as f:
        data = yaml.safe_load(f) or {}
    if "items" not in data:
        data["items"] = []
    return data


def main() -> int:
    defects = load_defects()
    items = defects.get("items", [])
    total_defects = len(items)
    loc = count_loc(SOURCE_ROOT)
    kloc = loc / 1000.0 if loc else 0.0
    ddr = (total_defects / kloc) if kloc > 0 else 0.0

    metrics = {
        "totalDefects": total_defects,
        "loc": loc,
        "kloc": round(kloc, 3),
        "ddr": round(ddr, 6),
        "timestamp": os.environ.get("GITHUB_RUN_ID") or "local"
    }

    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    with OUTPUT_FILE.open("w", encoding="utf-8") as f:
        json.dump(metrics, f, indent=2)

    print(f"Defect metrics written to {OUTPUT_FILE}")
    print(json.dumps(metrics, indent=2))

    threshold = os.environ.get("DDR_MAX")
    if threshold:
        try:
            max_ddr = float(threshold)
            if ddr > max_ddr:
                print(f"❌ DDR {ddr:.6f} exceeds threshold {max_ddr}")
                return 1
            else:
                print(f"✅ DDR {ddr:.6f} within threshold {max_ddr}")
        except ValueError:
            print("Warning: DDR_MAX invalid; ignoring threshold")
    return 0

if __name__ == "__main__":
    sys.exit(main())
