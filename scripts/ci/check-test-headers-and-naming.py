#!/usr/bin/env python3
"""
Phase 05 Test Headers and Naming Conformance Checker

Rules enforced (Phase 05 scope):
- Strict enforcement for tests under 05-implementation/**
  - Must contain a stable test identifier in header: TEST-UNIT-... or TEST-INT-...
  - Must include a Traceability block with Design: and Requirements: entries
  - Naming: unit tests should follow one of:
      * tests/unit/** paths OR filenames starting with test_*.{c,cpp,py,js,ts}
    integration tests should follow one of:
      * tests/integration/** paths OR filenames containing .int. or ending with _int.{c,cpp}
- Advisory (non-blocking) scan for repository-wide tests under tests/** to
  help migration; prints warnings but will not fail CI.

Exit codes:
- 0: All strict-scope tests conform OR none present
- 1: One or more strict-scope tests are non-conforming
"""
from __future__ import annotations

import os
import re
import sys
from typing import List, Tuple

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
STRICT_DIR = os.path.join(ROOT, "05-implementation")
REPO_TESTS_DIR = os.path.join(ROOT, "tests")

SRC_EXT = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".hxx", ".py", ".js", ".ts", ".tsx", ".jsx"}

IGNORES = [os.sep + "build" + os.sep, os.sep + "_deps" + os.sep, os.sep + "generated" + os.sep]


def is_ignored(path: str) -> bool:
    return any(p in path for p in IGNORES)


def normalize_header_lines(lines: List[str]) -> List[str]:
    cleaned: List[str] = []
    for line in lines:
        l = line.strip()
        l = re.sub(r"^(/\*+|\*+/?|//|#)", "", l).strip()
        cleaned.append(l)
    return cleaned


def read_head(path: str, max_lines: int = 120) -> List[str]:
    out: List[str] = []
    try:
        with open(path, "r", encoding="utf-8", errors="ignore") as f:
            for _ in range(max_lines):
                line = f.readline()
                if not line:
                    break
                out.append(line)
    except Exception:
        return []
    return out


def has_stable_test_id(lines: List[str]) -> bool:
    text = "\n".join(lines)
    return re.search(r"\bTEST-(UNIT|INT)-[A-Za-z0-9_-]+\b", text) is not None


def has_traceability(lines: List[str]) -> Tuple[bool, List[str]]:
    text = "\n".join(lines)
    missing: List[str] = []
    if re.search(r"^Traceability:\b", text, flags=re.IGNORECASE | re.MULTILINE) is None:
        missing.append("Traceability:")
    if re.search(r"^\s*Design:\b", text, flags=re.IGNORECASE | re.MULTILINE) is None:
        missing.append("Design:")
    if re.search(r"^\s*Requirements:\b", text, flags=re.IGNORECASE | re.MULTILINE) is None:
        missing.append("Requirements:")
    return (len(missing) == 0), missing


def classify_test_type(rel_path: str, filename: str) -> str:
    rp = rel_path.replace("\\", "/")
    name = filename.lower()
    if "/tests/integration/" in rp or ".int." in name or name.endswith("_int.cpp"):
        return "integration"
    if "/tests/unit/" in rp or name.startswith("test_"):
        return "unit"
    return "unknown"


def find_tests(start_dir: str) -> List[str]:
    found: List[str] = []
    if not os.path.isdir(start_dir):
        return found
    for root, _, files in os.walk(start_dir):
        for n in files:
            p = os.path.join(root, n)
            if is_ignored(p):
                continue
            _, ext = os.path.splitext(n)
            if ext.lower() not in SRC_EXT:
                continue
            # Heuristic: consider only files that look like tests by name or folder
            if "tests" in root.lower() or n.lower().startswith("test") or ".int." in n.lower():
                found.append(p)
    return found


def check_files(files: List[str], strict: bool) -> Tuple[List[Tuple[str, List[str]]], List[str]]:
    """Return (errors, warnings)."""
    errors: List[Tuple[str, List[str]]] = []
    warnings: List[str] = []

    for p in files:
        rel = os.path.relpath(p, ROOT)
        head = read_head(p)
        norm = normalize_header_lines(head)
        problems: List[str] = []

        if not has_stable_test_id(norm):
            problems.append("missing stable test identifier (TEST-UNIT-*/TEST-INT-*) in header")

        ok_tr, missing = has_traceability(norm)
        if not ok_tr:
            problems.extend([f"missing: {m}" for m in missing])

        # Naming classification
        name = os.path.basename(p)
        ttype = classify_test_type(rel, name)
        if ttype == "unknown":
            problems.append("naming/location does not match unit or integration test conventions")

        if problems:
            if strict:
                errors.append((rel, problems))
            else:
                warn_lines = "\n      - ".join(problems)
                warnings.append(f"{rel}\n      - {warn_lines}")

    return errors, warnings


def main() -> int:
    strict_tests = find_tests(STRICT_DIR)
    repo_tests = find_tests(REPO_TESTS_DIR)

    strict_errs, _ = check_files(strict_tests, strict=True)
    _, advisory_warns = check_files(repo_tests, strict=False)

    if strict_errs:
        print("❌ Phase 05 Test Headers/Naming Check FAILED for files under 05-implementation/**:\n")
        for rel, probs in strict_errs:
            print(f"- {rel}")
            for pr in probs:
                print(f"    • {pr}")
        print("\nAdd a header like (C++ example):\n")
        print("""// TEST-UNIT-<Name>\n// Traceability:\n//   Design: DES-C-001\n//   Requirements: REQ-F-001, REQ-NF-004\n""")
        print("And use naming such as tests/unit/test_<module>.cpp or tests/integration/<feature>.int.cpp")
        return 1

    if advisory_warns:
        print("⚠️ Test header/naming advisory (repository-wide): the following files don't yet match conventions. These are warnings only:\n")
        for w in advisory_warns:
            print(f"- {w}")

    if not strict_tests:
        print("✅ No Phase 05-scoped tests found under 05-implementation/** — nothing to enforce.")
    else:
        print("✅ Phase 05-scoped tests conform to header and naming rules.")

    return 0


if __name__ == "__main__":
    sys.exit(main())
