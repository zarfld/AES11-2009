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
- Reliability metrics instrumentation: utcFailures, dateTimeFailures, leapSecondFailures, timezoneFailures counters (see `lib/Standards/Common/reliability/metrics.*`). Use `ReliabilityMetrics::resetForTesting()` for deterministic test isolation. Additional structured event sink planned (Phase 05 incremental).
- Local coverage run (Linux/WSL/macOS with GCC/Clang):
  - Configure with coverage flags: `cmake -S . -B build -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug`
  - Build and test: `cmake --build build --config Debug && ctest --test-dir build -C Debug`
  - Generate report (example): `gcovr -r . --exclude 'tests/.*' --xml build/coverage.xml --html-details build/coverage.html`
  - Threshold check: ensure reported line coverage ≥ 80%.

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

## Traceability

- Maps REQ → ARC → DES → TEST → CODE for core flow. Each test case references corresponding code unit and design ID.
