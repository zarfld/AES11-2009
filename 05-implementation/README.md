# Phase 05 – Implementation Plan (AES-11 Standards Layer)

Status: ready to start

## Scope

Implement the hardware-agnostic AES-11-2009 DARS protocol and supporting components per IEEE/ISO lifecycle and the completed design specs:

- DES-C-001 DARS Protocol Core
- DES-C-002 Synchronization Manager
- DES-C-003 Timing Window Processor
- DES-I-001/002/003/004 Interfaces
- DES-D-001/002 Data Models

All code in this phase remains vendor/OS-agnostic and testable without hardware.

## Tech stack

- Language: C++17 (portable)
- Build: CMake
- Tests: GoogleTest (unit); specs mapped from TEST-* documents

## CI and Quality Gates

- GitHub Actions workflow (`.github/workflows/ci.yml`) builds on Linux and Windows, runs all tests, and enforces line coverage ≥ 80% on Linux via gcovr.
- Static analysis:
  - `clang-tidy` (minimal checks: `clang-analyzer-*`, `bugprone-*`) runs in CI using the repository `.clang-tidy` config and CMake `compile_commands.json`. Violations fail the build.
  - `cppcheck` runs in the standards compliance workflow with `--enable=warning,style,performance,portability`; violations fail the build (inline suppressions allowed).
- Reliability metrics instrumentation: utcFailures, dateTimeFailures, leapSecondFailures, timezoneFailures counters (see `lib/Standards/Common/reliability/metrics.*`). Use `ReliabilityMetrics::resetForTesting()` for deterministic test isolation. Additional structured event sink planned (Phase 05 incremental).
- Structured reliability event sink: register a process-wide sink via `Common::reliability::set_event_sink()` (see `lib/Standards/Common/reliability/events.*`). Metrics increments emit events (e.g., `utc_failure`, `timezone_failure`, `leap_second_failure`) as lightweight, hardware-agnostic structured signals. If no sink is set, emission is a no-op.
- Local coverage run (Linux/WSL/macOS with GCC/Clang):
  - Configure with coverage flags: `cmake -S . -B build -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug`
  - Build and test: `cmake --build build --config Debug && ctest --test-dir build -C Debug`
  - Generate report (example): `gcovr -r . --exclude 'tests/.*' --xml build/coverage.xml --html-details build/coverage.html`
  - Threshold check: ensure reported line coverage ≥ 80%.

### Suppressing specific findings (with justification)

- Clang-Tidy:
  - Suppress next line only: `// NOLINTNEXTLINE(clang-analyzer-core.CallAndMessage)  // reason`
  - Suppress at symbol: `// NOLINT(bugprone-branch-clone)  // reason`
- Cppcheck:
  - Inline suppression: `// cppcheck-suppress nullPointer  // reason`

Please prefer refactoring to suppressions. When suppression is necessary, document the rationale clearly and link to the relevant design/requirement ID.

## Directory layout

lib/Standards/

- AES/AES11/2009/
  - core/ (protocol core, synchronization manager, timing reference point)
  - dars/ (DARS generation/validation)
  - sync/ (selection, degradation, holdover)
  - video/ (video alignment stubs)
  - conformity/ (conformance hooks)
- Common/
  - interfaces/ (abstract interfaces for tests)
  - utils/
  - timing/
  - testing/

## Milestones (TDD)

Milestone 1: Scaffolding

- Top-level CMakeLists and library targets for aes11_2009 and standards_common
- Namespaces and headers with docblocks referencing AES sections (numbers only)

Milestone 2: Timing primitives

- Implement DES-I-002 mock (deterministic monotonic clock + sample counter)
- Tests: TEST-TIMESRC-SNAPSHOT-001/002

Milestone 3: Timing Window Processor

- Implement DES-C-003 (Welford O(1) aggregates, circular window)
- Tests: TEST-DM-TIMINGWIN-001/002/003; degradation thresholds

Milestone 4: Protocol core (skeleton)

- DES-C-001 state enums, TRP handling, lock acquisition basics
- Tests: TEST-DARS-STATE-001; basic jitter acceptance

Milestone 5: Synchronization manager (skeleton)

- DES-C-002 selection by priority + stability, hysteresis, forced reselection, degradation flags, holdover drift
- Tests: TEST-SYNC-SELECT-001, TEST-SYNC-DEGRADE-001, TEST-SYNC-RESELECT-001, TEST-SYNC-HOLDOVER-001

## Acceptance criteria for Phase 05 start

- CMake config builds on a clean machine without external vendor deps
- Unit test runner executes and discovers placeholder tests
- Coding guideline: no dynamic allocation in hot paths; lock-free O(1) operations
- Namespaces and folders match Standards instructions (AES::AES11::_2009)

## Notes

- External AES3/AES5 repos may be added later via FetchContent; not required to start
- All comments must avoid reproducing copyrighted standard text; reference section numbers only
- Reliability hooks: expose error paths via return codes; tests assert failure cases (e.g., buffer length guard, leap-second rule). Structured logging can be added in service layer; standards layer remains pure and hardware-agnostic.
- Extended reliability counters added for leap-second rule violations and timezone range failures to support IEEE 1633 evidence collection. Future enhancement: lightweight event sink for external aggregation.

## Reliability artifacts (Phase 05 evidence)

- Operational Profile (OP): see `docs/reliability/operational-profile-AES11-DARS-2009.md`
- SFMEA: see `docs/reliability/sfmea-AES11-DARS-2009.md`
- Critical Items List (CIL): see `docs/reliability/cil-AES11-DARS-2009.md`
- Defect Log: see `docs/reliability/defect-log.md` (machine-readable: `docs/reliability/defect-log.yaml`)
- Coverage badge (CI): `docs/badges/coverage.svg`

These artifacts support IEEE 1633 reliability program activities and Phase 06/07 data collection.

## Traceability & reports

- Requirements traceability matrix: `reports/traceability-matrix.md` (kept in-repo and refreshed by CI)
- CI-generated trace JSON (artifact): `build/traceability.json` (download from CI artifacts)
- Spec index (artifact): `build/spec-index.json`

### Regenerate traceability locally (Windows PowerShell)

```powershell
py -3 scripts/validate-spec-structure.py
py -3 scripts/generators/spec_parser.py
py -3 scripts/generators/build_trace_json.py
py -3 scripts/validate-trace-coverage.py --min-req 90 --min-req-adr 70 --min-req-scenario 60 --min-req-test 40
py -3 scripts/generate-traceability-matrix.py
```

## Traceability

- Maps REQ → ARC → DES → TEST → CODE for core flow. Each test case references corresponding code unit and design ID.
