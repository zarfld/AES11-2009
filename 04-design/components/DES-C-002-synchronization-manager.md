---
title: "Software Design Description - Synchronization Manager"
specType: design
version: 0.1.0
status: draft
author: "AI Assistant"
date: "2025-11-07"
relatedArchitecture:
  - ARC-C-004
  - ARC-C-001
relatedRequirements:
  - REQ-F-SYNC-001
  - REQ-F-SYNC-002
  - REQ-F-SYNC-003
  - REQ-F-SYNC-004
  - REQ-NF-PERF-001
  - REQ-NF-REL-001
relatedADRs:
  - ADR-003
  - ADR-004
traceabilityTier: component
---
## DES-C-002 Synchronization Manager

## 1. Purpose

Coordinates system-wide synchronization using DARS protocol core (DES-C-001) and timing sources (GPS 1PPS, video sync, internal clock) enforcing locking policies and fallback strategies.

## 2. Responsibilities

- Orchestrate lock acquisition sequence among multiple timing sources.
- Apply priority and fallback order (Video > External PPS > Internal oscillator).
- Monitor stability metrics; escalate anomalies to conformance harness.
- Provide unified API for current master reference and health status.

## 3. Provided Interface Summary

`ISynchronizationManager` (DES-I-003 planned):

- `configure(sources)` – register available timing sources.
- `select_master()` – choose optimal source based on stability & priority.
- `poll()` – periodic update; assess drift & jitter.
- `get_status()` – rich struct with master source, state, metrics.
- `force_reselection()` – manual override to re-run selection algorithm.

## 4. Required Interfaces

- Timing source abstraction (DES-I-002).
- DARS protocol core (DES-C-001 via IDARSProtocolCore).
- Platform capability query (future DES-I-004) for hardware timestamp support.

## 5. Data Structures

### 5.1 `SyncStatus`

```cpp
struct SyncStatus {
    DARSState dars_state;      // from protocol core
    const char* master_source; // symbolic name
    uint32_t master_quality_ppm; // instantaneous quality metric
    uint32_t holdover_ppm;     // projected holdover stability
    bool degraded;             // true if below performance threshold
};
```
 
### 5.2 `TimingSource`

```cpp
struct TimingSource {
    const char* name;
    uint8_t priority;              // lower = higher priority
    bool available;                // dynamic availability flag
    uint32_t stability_window_ppm; // recent performance metric
    bool supports_video_alignment; // optimization flag
};
```

## 6. Algorithms

### 6.1 Master Selection

1. Filter available sources.
2. Rank by priority then by stability_window_ppm.
3. Require stability threshold < configured limit (e.g., 5 ppm for Grade 1 pre-lock).
4. Validate DARS core lock before confirming selection.
5. Emit event for change; update SyncStatus.

### 6.1.1 Phase Tolerance Integration (DES-DARS-PHASE)

- Use `PhaseTolerance::within_output(sr, |offset_us|)` as a predicate to maintain output phase within ±5% (REQ-F-DARS-004).
- Use `PhaseTolerance::within_input(sr, |offset_us|)` to accept input phase up to ±25% (REQ-F-DARS-004).
- When `|offset_us|` crosses `output_warning_threshold_us(sr)` persistently, trigger proactive realignment without dropping lock.
- Expose counters for phase violations to support monitoring (ties to REQ-F-DARS-004 error handling table).

### 6.2 Degradation Detection

- If jitter metrics exceed threshold OR stability drops > tolerance for N consecutive polls → mark `degraded=true` and notify conformance subsystem.

### 6.3 Holdover Strategy

- Capture last stable TRP rate; compute drift compensation factor.
- Maintain predictive error bound; if exceeds threshold escalate to reacquiring.

## 7. Error Handling

- Non-fatal: transient unavailability of a source → soft log + fallback.
- Fatal: no sources available → system enters `Acquiring` with degraded flag.
- Provide enumerated error codes mapped to requirement references.

## 8. Performance

- `poll()` O(S) where S = number of sources (expected small <5).
- Selection algorithm invoked on events; average cost trivial vs frame processing.
- Avoid heap allocations; arrays sized at configuration time.

## 9. Security & Integrity

- Validate source metadata; reject inconsistent priority values.
- Prevent malicious source spoofing by cross-checking stability metrics against DARS core.

## 10. Traceability

| Requirement | Mechanism |
|-------------|-----------|
| REQ-F-SYNC-001 | Master selection algorithm & stability checks |
| REQ-F-SYNC-002 | Multi-source orchestration & fallback sequence |
| REQ-F-SYNC-003 | Degradation detection & status reporting |
| REQ-F-SYNC-004 | Holdover strategy & drift compensation |
| REQ-NF-PERF-001 | Bounded O(S) algorithms & minimal per-poll cost |
| REQ-NF-REL-001 | Automatic fallback + degraded mode signaling |

## 11. Test Strategy (Planned)

- TEST-SYNC-SELECT-001: Select highest priority stable source.
- TEST-SYNC-DEGRADE-001: Mark degraded on sustained jitter spike.
- TEST-SYNC-HOLDOVER-001: Correct drift during holdover within ppm bounds.
- TEST-SYNC-RESELECT-001: Force reselection chooses next viable source.
- TEST-SYNC-PHASE-001: Maintain output phase within ±5% at 48 kHz (Verifies: REQ-F-DARS-004).
- TEST-SYNC-PHASE-002: Accept input phase within ±25% at 48 kHz (Verifies: REQ-F-DARS-004).

## 12. Open Issues

- Whether to integrate predictive filtering (Kalman) → deferred (YAGNI).
- Video alignment deep integration vs plugin approach.

## 13. Refactoring Opportunities

- Replace string `name` with interned ID for faster comparisons.
- Consolidate selection and degradation thresholds into configuration struct.

## 14. References

- ADR-003 (timing abstraction), ADR-004 (memory), ADR-001 (layering).
