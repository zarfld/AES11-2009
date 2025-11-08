#!/usr/bin/env python3
"""
Build C++ with coverage flags, run tests, and generate gcovr coverage reports.

Outputs:
- build/coverage/coverage.json (gcovr JSON)
- build/coverage/coverage.xml (Cobertura XML)
- build/coverage/summary.json (our summary: lines_valid, lines_covered, percent)
- build/coverage/coverage.txt (human-readable summary)
"""
from __future__ import annotations

import json
import os
import subprocess
import sys
from pathlib import Path


def run(cmd: list[str], cwd: str | None = None) -> None:
    print("$", " ".join(cmd))
    subprocess.check_call(cmd, cwd=cwd)


def main() -> int:
    root = Path(__file__).resolve().parents[2]
    build_dir = root / "build-coverage"
    out_dir = root / "build" / "coverage"
    out_dir.mkdir(parents=True, exist_ok=True)

    # Configure with coverage flags
    cmake_cmd = [
        "cmake",
        "-S",
        str(root),
        "-B",
        str(build_dir),
        "-DCMAKE_BUILD_TYPE=Debug",
        "-DBUILD_TESTS=ON",
        # Force GCC to ensure gcov-compatible artifacts on ubuntu-latest
        "-DCMAKE_C_COMPILER=gcc",
        "-DCMAKE_CXX_COMPILER=g++",
        "-DCMAKE_C_FLAGS=--coverage",
        "-DCMAKE_CXX_FLAGS=--coverage",
        "-DCMAKE_EXE_LINKER_FLAGS=--coverage",
        "-DCMAKE_SHARED_LINKER_FLAGS=--coverage",
    ]
    run(cmake_cmd)
    # Build
    run(["cmake", "--build", str(build_dir), "--", f"-j{os.cpu_count() or 2}"])
    # Run tests
    run(["ctest", "--test-dir", str(build_dir), "--output-on-failure"])

    # Install gcovr if missing
    try:
        subprocess.check_call(["python3", "-c", "import gcovr"], stdout=subprocess.DEVNULL)
    except subprocess.CalledProcessError:
        subprocess.check_call(["pip3", "install", "--user", "gcovr"])
    gcovr_bin = os.path.expanduser("~/.local/bin/gcovr")
    if not Path(gcovr_bin).exists():
        gcovr_bin = "gcovr"

    # Generate reports
    json_path = out_dir / "coverage.json"
    xml_path = out_dir / "coverage.xml"
    txt_path = out_dir / "coverage.txt"
    run([
        gcovr_bin,
        "-r",
        str(root),
        "--object-directory",
        str(build_dir),
        "--exclude-directories",
        "external",
        "--json",
        str(json_path),
    ])
    run([
        gcovr_bin,
        "-r",
        str(root),
        "--object-directory",
        str(build_dir),
        "--exclude-directories",
        "external",
        "--xml",
        str(xml_path),
    ])
    # Human readable summary
    with open(txt_path, "w", encoding="utf-8") as ftxt:
        subprocess.check_call([
            gcovr_bin,
            "-r",
            str(root),
            "--object-directory",
            str(build_dir),
            "--exclude-directories",
            "external",
        ], stdout=ftxt)

    # Summarize
    data = json.loads(json_path.read_text(encoding="utf-8"))
    total_valid = 0
    total_covered = 0
    for f in data.get("files", []):
        summary = f.get("summary", {})
        total_valid += int(summary.get("lines", {}).get("count", 0))
        total_covered += int(summary.get("lines", {}).get("covered", 0))
    percent = round((total_covered / total_valid) * 100.0, 2) if total_valid else 0.0
    (out_dir / "summary.json").write_text(
        json.dumps({
            "lines_valid": total_valid,
            "lines_covered": total_covered,
            "line_coverage_percent": percent,
        }, indent=2),
        encoding="utf-8",
    )
    print(f"Coverage: {percent}% ({total_covered}/{total_valid})")
    return 0


if __name__ == "__main__":
    sys.exit(main())
