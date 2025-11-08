#!/usr/bin/env python3
"""
Phase 05 Traceability Header Checker

Enforces that all source/test files under 05-implementation/** contain a top-of-file
traceability comment block with the following minimum keys:

- Phase: 05-implementation
- Traceability:
    - Design:
    - Requirements:
    - Tests:

Accepted comment styles: C/C++ (//, /* */), Python (#, """"""), JS/TS (//, /* */).
Markdown and spec files are excluded in this phase by design (YAML schemas apply to 02–04).

Exit codes:
- 0: All files compliant or no applicable files found
- 1: One or more files missing required header elements

This script is intentionally strict for Phase 05 enforcement per
.github/instructions/phase-05-implementation.instructions.md
"""
from __future__ import annotations

import os
import re
import sys
from typing import List, Tuple

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
TARGET_DIR = os.path.join(ROOT, "05-implementation")

# File extensions considered source/tests for Phase 05 enforcement
SRC_EXT = {
    ".c", ".cc", ".cpp", ".cxx",
    ".h", ".hh", ".hpp", ".hxx",
    ".ixx",
    ".py", ".js", ".ts", ".tsx", ".jsx",
}

# Paths to ignore under 05-implementation
IGNORE_PATTERNS = [
    os.sep + "build" + os.sep,
    os.sep + "_deps" + os.sep,
    os.sep + "generated" + os.sep,
]


def is_ignored(path: str) -> bool:
    for patt in IGNORE_PATTERNS:
        if patt in path:
            return True
    return False


def normalize_header_lines(lines: List[str]) -> List[str]:
    """Strip common comment markers from the beginning of lines and whitespace."""
    cleaned = []
    for line in lines:
        l = line.strip()
        # Remove leading comment tokens
        l = re.sub(r"^(/\*+|\*+/?|//|#)", "", l).strip()
        cleaned.append(l)
    return cleaned


def has_required_keys(lines: List[str]) -> Tuple[bool, List[str]]:
    """Check that required keys are present in the first N lines."""
    missing: List[str] = []
    text = "\n".join(lines)

    def present(pattern: str) -> bool:
        return re.search(pattern, text, flags=re.IGNORECASE) is not None

    if not present(r"^Phase:\s*05-implementation\b"):
        missing.append("Phase: 05-implementation")
    if not present(r"^Traceability:\b"):
        missing.append("Traceability:")
    if not present(r"^\s*Design:\b"):
        missing.append("Design:")
    if not present(r"^\s*Requirements:\b"):
        missing.append("Requirements:")
    if not present(r"^\s*Tests:\b"):
        missing.append("Tests:")

    return (len(missing) == 0), missing


def scan_file(path: str) -> Tuple[bool, List[str]]:
    # Read only the first 100 logical lines; enough for header blocks
    try:
        with open(path, "r", encoding="utf-8", errors="ignore") as f:
            head = [next(f) for _ in range(100)]
    except StopIteration:
        # File shorter than 100 lines; that's fine
        pass
    except Exception as e:
        return False, [f"Error reading file: {e}"]

    try:
        with open(path, "r", encoding="utf-8", errors="ignore") as f:
            head = []
            for _ in range(100):
                line = f.readline()
                if not line:
                    break
                head.append(line)
    except Exception as e:
        return False, [f"Error reading file: {e}"]

    normalized = normalize_header_lines(head)
    ok, missing = has_required_keys(normalized)
    return ok, missing


def main() -> int:
    if not os.path.isdir(TARGET_DIR):
        print(f"No 05-implementation directory found at {TARGET_DIR}; nothing to enforce.")
        return 0

    non_compliant: List[Tuple[str, List[str]]] = []

    for root, _, files in os.walk(TARGET_DIR):
        for name in files:
            path = os.path.join(root, name)
            if is_ignored(path):
                continue
            _, ext = os.path.splitext(name)
            if ext.lower() not in SRC_EXT:
                # Skip markdown/spec and other non-source files in Phase 05
                continue
            ok, missing = scan_file(path)
            if not ok:
                rel = os.path.relpath(path, ROOT)
                non_compliant.append((rel, missing))

    if non_compliant:
        print("❌ Phase 05 Traceability Header Check FAILED. The following files are missing required elements:\n")
        for rel, missing in non_compliant:
            print(f"- {rel}")
            for m in missing:
                print(f"    • missing: {m}")
        print("\nPlease add a top-of-file traceability comment block, e.g. (C++):\n")
        print("""/*
Module: <relative/path/to/file>
Phase: 05-implementation
Traceability:
  Design: DES-C-001
  Requirements: REQ-F-001, REQ-NF-004
  Tests: TEST-UNIT-XYZ
Notes: Keep IDs current when refactoring; maintain links in tests.
*/
""")
        return 1

    print("✅ Phase 05 Traceability Header Check passed (no missing headers).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
