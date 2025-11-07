---
id: DES-DARS-PHASE
title: DARS Phase Tolerance Component
status: draft
version: 0.1.0
owner: standards-team
implements:
  - REQ-F-DARS-004
supports:
  - REQ-F-DARS-002
---

## Purpose
 
Provides phase relationship evaluation for Timing Reference Point (TRP) alignment across inputs and outputs, enforcing ±5% output and ±25% input tolerances defined by AES-11 understanding (no reproduction of spec text). Supplies warning threshold (±4.5%) for proactive realignment.

## Context

* Output connectors must remain tightly phase-aligned to prevent sample slips and jitter accumulation.
* Input acceptance range is broader to allow interoperability with devices having moderate phase offsets.
* Acquisition / synchronization manager requires predicates for: within tolerance, approaching violation (warning), and exceeded tolerance (trigger corrective realignment).

## Requirements Traceability

| Requirement | Relationship | Notes |
|-------------|-------------|-------|
| REQ-F-DARS-004 | Implements | Output ±5% / Input ±25% of frame period, warning at ±4.5% output |
| REQ-F-DARS-002 | Supports | Accurate frequency measurement contextualizes microsecond offsets |

## Interfaces

### Public API

* `PhaseTolerance::frame_period_us(sampleRateHz)` → microseconds per AES3 frame
* `PhaseTolerance::output_tolerance_us(sampleRateHz)` → microseconds (±5%)
* `PhaseTolerance::input_tolerance_us(sampleRateHz)` → microseconds (±25%)
* `PhaseTolerance::output_warning_threshold_us(sampleRateHz)` → microseconds (±4.5%)
* `PhaseTolerance::within_output(sampleRateHz, absPhaseOffsetUs)` → bool
* `PhaseTolerance::within_input(sampleRateHz, absPhaseOffsetUs)` → bool

## Design Rationale

* Pure functions keep evaluation deterministic and trivially testable.
* Express tolerances as derived values from frame period to avoid hardcoded numbers for each sampling frequency.
* Warning threshold surfaced separately to allow synchronization logic to differentiate proactive vs reactive adjustments.

## Algorithm

1. Compute frame period: `1e6 / sampleRateHz`.
2. Derive tolerances by multiplying period by 0.05 (output) and 0.25 (input).
3. Compare absolute measured phase offset to derived tolerance.
4. Provide warning predicate using 0.045 multiplier for output.

## Data Structures

None beyond static methods; stateless utility class pattern for consistency with capture range component.

## Error Handling

* Non-positive sampling rate returns zero tolerance; predicates fail (invalid context). Caller responsible for ensuring valid rate.

## Concurrency

* Stateless utilities are thread-safe; no synchronization required.

## Testing Strategy

| Test ID | Purpose | Requirement |
|---------|---------|-------------|
| TEST-DARS-PHASE-001 | Output tolerance boundary at 48 kHz (exact ±5% vs >5%) | REQ-F-DARS-004 |
| TEST-DARS-PHASE-002 | Input tolerance boundary at 48 kHz (exact ±25% vs >25%) | REQ-F-DARS-004 |
| TEST-DARS-PHASE-003 | Warning threshold detection around ±4.5% output | REQ-F-DARS-004 |

## Future Extensions

* Add diagnostic function returning structured phase evaluation result (tolerance status, warning flag, violation magnitude).
* Integrate with synchronization manager to perform automatic phase realignment when warning threshold crossed persistently.
* Extend to multi-channel phase skew aggregation.

## Traceability Notes
 
This component provides concrete implementation logic for REQ-F-DARS-004 enabling authentic linkage through tests and requirement forward references; it supports REQ-F-DARS-002 by relying on accurate sampling rate measurement to contextualize microsecond offsets.
