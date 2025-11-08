---
title: "SFMEA - AES11 DARS Protocol"
specType: sfmea
standard: IEEE 1633-2016
phase: 05-implementation
status: draft
version: 0.1.0
component: AES::AES11::_2009::dars
author: GitHub Copilot AI
created: 2025-11-08
updated: 2025-11-08
traceability:
  design: [DES-C-001, DES-C-002, DES-C-003]
  requirements: [REQ-F-DARS-GEN-001, REQ-F-DARS-VAL-002, REQ-NF-RELIABILITY-001]
  operationalProfile: docs/reliability/operational-profile-AES11-DARS-2009.md
copyrightNotice: |
  Original analysis based on understanding of AES-11-2009 and IEEE 1633 reliability guidance. No copyrighted specification text reproduced.
---

## 1. Purpose

Software Failure Modes and Effects Analysis (SFMEA) for the hardware-agnostic AES-11-2009 DARS generation & validation component. Supports early identification of high-risk failure modes and creation of the Critical Items List (CIL).

## 2. Rating Scales

| Dimension | Scale 1 (Low Impact) | Scale 10 (High Impact) |
|-----------|----------------------|------------------------|
| Severity (S) | No user-visible effect | Total loss of synchronization / mission failure |
| Occurrence (O) | Improbable (<1e-6/hr) | Frequent (>1e-2/hr under nominal) |
| Detectability (D) | Automatic detection & isolation | Silent, no direct symptom until consequence |

RPN = S × O × D. Threshold for CIL inclusion: RPN ≥ 200 (per internal risk policy).

## 3. Failure Mode Table

| FMEA ID | Function | Failure Mode | Category | Cause | Effect | S | O | D | RPN | Current Controls | Recommended Mitigation | Trace Tests |
|---------|----------|--------------|----------|-------|--------|---|---|---|-----|------------------|------------------------|------------|
| FM-001 | Frame Generation (OP-002) | Incorrect preamble alignment (X/Z mismatch) | Sequencing | Logic error in reference point calc | Downstream devices reject frame; sync instability | 8 | 4 | 5 | 160 | Unit tests (basic) | Add edge-case tests & assert alignment tolerance | TEST-DARS-STATE-001 |
| FM-002 | Frame Generation (OP-002) | Timing reference drift beyond Grade 1 tolerance | Timing | Accumulated rounding error in drift compensation | Grade misclassification; potential jitter | 9 | 5 | 6 | 270 | Basic ppm checks | Implement continuous drift monitor & correction | TEST-DM-TIMINGWIN-001 |
| FM-003 | Frame Validation (OP-003) | Misinterpret DARS identification bits | Data | Bit mask misuse | Accept non-DARS frames; false sync | 7 | 3 | 5 | 105 | Bit mask unit test | Strengthen mask validation & negative tests | TEST-DARS-STATE-001 |
| FM-004 | Holdover (OP-005/006) | Holdover drift exceeds Grade 2 tolerance | Timing | Inadequate drift modeling / stale baseline | Degraded audio timing; downstream resync bursts | 8 | 4 | 7 | 224 | Basic drift counter | Add adaptive drift estimator & alert emission | TEST-SYNC-HOLDOVER-001 |
| FM-005 | Source Reselect (OP-007) | Race condition yields inconsistent timing ref | Timing/Sequencing | Concurrent updates across threads (future) | Glitch in timing; potential frame discontinuity | 9 | 3 | 7 | 189 | Single-thread assumption | Introduce atomic transition & state lock | TEST-SYNC-RESELECT-001 |
| FM-006 | Fault Injection (OP-008) | Residual fault active in production build | Error Handling | Flag not cleared / compile flag leak | Artificial failures in production metrics | 6 | 2 | 9 | 108 | Compile-time guard | Add runtime sanity check & CI build audit | TEST-TIMESRC-SNAPSHOT-001 |
| FM-007 | Metrics (OP-009) | Counter overflow (e.g., uint32 wrap) | Data | Long mission without rollover handling | Misleading reliability data; MTBF miscalc | 5 | 5 | 8 | 200 | 32-bit counters | Switch to 64-bit & overflow detection log | TEST-TIMESRC-SNAPSHOT-002 |
| FM-008 | Drift Assessment (OP-010) | Integer overflow in jitter/drift ppm math | Logic | Large intermediate multiplication | Incorrect tolerance decisions | 8 | 4 | 6 | 192 | Range assertions absent | Use 128-bit intermediate or checked math | TEST-DM-TIMINGWIN-001 |
| FM-009 | Leap Second Handling | Leap second misapplied (duplicate / missing) | Sequencing/Timing | Off-by-one in time conversion | Off-tolerance reference; sync glitch | 7 | 4 | 7 | 196 | Minimal validation | Add explicit leap second table sanity checks | TEST-SYNC-SELECT-001 |
| FM-010 | Channel Status Parse | Buffer overrun parsing channel status | Data | Bounds miscalculation | Memory corruption / undefined behavior | 10 | 2 | 6 | 120 | Basic length checks | Strengthen length & index assertions + fuzz tests | TEST-DARS-STATE-001 |
| FM-011 | Degradation Detection | Failure to detect entering degraded state | Error Handling | Missed condition due to threshold miscompare | Unlogged prolonged performance loss | 8 | 3 | 8 | 192 | Single threshold check | Dual threshold hysteresis & health metric | TEST-SYNC-DEGRADE-001 |
| FM-012 | Transition Logging | Transition event missing (lost) | Sequencing | Omitted increment path | Probability model inaccurate | 4 | 5 | 8 | 160 | None (early phase) | Add mandatory macro around transitions | (Planned) |
| FM-013 | Holdover Aging | Aging model underestimates drift | Data/Timing | Simplistic linear model | Unexpected tolerance breach | 8 | 4 | 6 | 192 | None (not implemented) | Implement empirical aging factor calibration | TEST-SYNC-HOLDOVER-001 |
| FM-014 | Fault Injection Registry | Concurrency issues (future multi-thread) | Sequencing | Non-atomic access | Inconsistent fault activation state | 6 | 3 | 7 | 126 | Current single-thread | Plan thread-safe atomic wrapper | TEST-FAULT-INJECTION (existing) |

## 4. High-Risk Summary (RPN ≥ 200)

| FMEA ID | RPN | Driver | Inclusion Rationale |
|---------|-----|--------|---------------------|
| FM-002 | 270 | Timing drift | Direct impact on Grade 1 compliance |
| FM-004 | 224 | Holdover drift | Risk of prolonged degraded accuracy |
| FM-007 | 200 | Metrics integrity | Data integrity for reliability analytics |

These items form the initial Critical Items List (see `cil-AES11-DARS-2009.md`).

## 5. Mitigation Plan Overview

| FMEA ID | Immediate Action | Longer-Term Action | Verification |
|---------|------------------|--------------------|-------------|
| FM-002 | Add continuous drift monitor counter & ppm recalculation each N frames | Statistical drift predictor calibration with real telemetry | Extended timing window tests + stress |
| FM-004 | Implement adaptive holdover drift estimator & jitter alarm | Integrate PTP alternative baseline (future) | Holdover simulation test harness |
| FM-007 | Promote counters to 64-bit; add overflow log & test | Periodic snapshot & reset mechanism for long missions | Counter stress test (synthetic loop) |

## 6. Open Items

| Item ID | Description | Linked FMEA | Target Phase | Status |
|---------|-------------|-------------|-------------|--------|
| SFMEA-OP-COUNTERS | Transition counters instrumentation | FM-012 | Phase 06 | Planned |
| SFMEA-DRIFT-MON | Continuous drift monitor implementation | FM-002 | Phase 05 | In-Progress |
| SFMEA-HOLD-EST | Adaptive holdover estimator | FM-004 | Phase 06 | Planned |
| SFMEA-MET-64 | Upgrade metrics counters to 64-bit | FM-007 | Phase 05 | Planned |
| SFMEA-LEAP-CHK | Leap second table validation | FM-009 | Phase 05 | Planned |

## 7. Assumptions

- Current implementation single-threaded; concurrency failures (FM-005, FM-014) deferred until multithreading introduced.
- External time sources abstracted; hardware-specific drift anomalies out of scope.
- RPN values preliminary; will be recalibrated with empirical occurrence data in Phase 06.

## 8. References

- Operational Profile: `operational-profile-AES11-DARS-2009.md`
- Reliability Requirements (internal docs)
- IEEE 1633-2016 methodology (conceptual reference)

## 9. Compliance Notes

All analyses are original interpretive content intended to guide internal reliability engineering. No reproduction of AES or IEEE proprietary text.

