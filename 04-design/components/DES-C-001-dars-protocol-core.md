---
title: "Software Design Description - DARS Protocol Core"
specType: design
version: 0.1.0
status: draft
author: "AI Assistant"
date: "2025-11-07"
relatedArchitecture:
  - ARC-C-001
  - ARC-C-002
  - ARC-C-004
relatedRequirements:
  - REQ-F-DARS-001
  - REQ-F-DARS-002
  - REQ-F-DARS-003
  - REQ-F-SYNC-001
  - REQ-NF-PERF-001
relatedADRs:
  - ADR-001
  - ADR-002
  - ADR-003
  - ADR-004
traceabilityTier: component
---
## DES-C-001 DARS Protocol Core

## 1. Purpose and Scope
Implements the core AES-11-2009 Digital Audio Reference Signal (DARS) protocol logic including state machine management (acquire, lock, holdover), timing reference handling, and integration with AES3/AES5 abstractions. Excludes hardware-specific concerns (delegated via interfaces).

## 2. Responsibilities

- Manage DARS synchronization state transitions (REQ-F-DARS-001/002)
- Generate timing reference points (TRP) per spec understanding (REQ-F-DARS-003)
- Expose lock status and accuracy metrics (REQ-F-SYNC-001, REQ-NF-PERF-001)
- Provide callback hooks for conformance harness

## 3. Provided Interfaces

- `IDARSProtocolCore` (DES-I-001) methods:
  - `initialize(config)` – configure sample rate, timing source abstraction
  - `update(frameCtx)` – process AES3 frame context and advance state machine
  - `get_lock_status()` – returns enum {ACQUIRING, LOCKED, HOLDOVER, ERROR}
  - `get_timing_reference()` – returns current TRP (64-bit timestamp)
  - `get_jitter_metrics()` – returns struct of peak/mean jitter

## 4. Required Interfaces

- Timing source (DES-I-002) for high-resolution time and sample counters
- AES3 frame provider (external AES3 repo) for frame parsing
- Sampling frequency data (external AES5 repo) for validation

## 5. Design Constraints

- Real-time safe: no dynamic allocation in `update()` path (ADR-004)
- Deterministic execution budget: < 50 µs per frame processing on target CPU
- Lock acquisition capture ranges derived from spec understanding (Grade 1 ±2 ppm)
- Portable C++17 subset; avoid exceptions in hot path

## 6. Data Structures

### 6.1 `DARSState`

```cpp
enum class DARSState : uint8_t { Acquiring, Locked, Holdover, Error };
```

### 6.2 `JitterMetrics`

```cpp
struct JitterMetrics {
    uint32_t peak_ppm;    // peak deviation in ppm
    uint32_t mean_ppm;    // mean deviation sampled over window
    uint32_t window_samples; // number of samples in measurement window
};
```

### 6.3 `FrameContext`

Minimal info required from AES3 frame to process TRP:

```cpp
struct FrameContext {
    uint64_t frame_index;        // sequential frame count
    uint64_t sample_time_ns;     // high-resolution time for frame start
    bool     is_trp_boundary;    // derived from preamble detection
    uint32_t sample_rate_hz;     // validated against AES5 constants
};
```

## 7. Algorithms and State Machine

### 7.1 State Transition Logic (Simplified)

```text
Acquiring -> Locked      when stable TRP interval within tolerance window
Locked    -> Holdover    when timing source unavailable but previous stability
Holdover  -> Acquiring   when deviation exceeds holdover tolerance or recovery
Any       -> Error       on protocol violation (invalid TRP, rate mismatch)
```

### 7.2 TRP Generation

- Detect TRP via AES3 preamble (X/Z transition) and mark boundary
- Compute `timing_reference = base_time + (frame_index * samples_per_frame / sample_rate)`
- Validate against expected interval; update jitter metrics

### 7.3 Jitter Measurement

- Maintain circular buffer of last N (e.g., 256) TRP deltas
- Peak ppm = max(|delta - nominal| / nominal * 1e6)
- Mean ppm = average(|delta - nominal| / nominal * 1e6)

## 8. Error Handling

- Return status codes via `get_lock_status()`; provide separate error code query
- Distinguish recoverable (transient frame anomaly) vs non-recoverable (config mismatch)
- Log (deferred to service layer) with requirement references in metadata

## 9. Performance Considerations

- All per-frame operations O(1); circular buffers fixed-size
- Avoid division in hot path via precomputed reciprocals for sample rate
- Provide instrumentation hooks compiled out in release

## 10. Security Considerations

- Validate all external inputs: sample rate, frame indices, TRP boundaries
- Reject inconsistent timing source data to prevent spoofed lock conditions

## 11. Traceability Mapping

| Requirement | Mechanism |
|-------------|-----------|
| REQ-F-DARS-001 | State machine acquiring logic |
| REQ-F-DARS-002 | Lock stabilization criteria and transitions |
| REQ-F-DARS-003 | TRP detection and generation algorithm |
| REQ-F-SYNC-001 | Public timing reference accessor |
| REQ-NF-PERF-001 | Execution budget & jitter metrics monitoring |

## 12. Test-Driven Design Hooks

Planned test IDs:

- TEST-DARS-STATE-001: Acquiring → Locked transition on stable interval
- TEST-DARS-STATE-002: Locked → Holdover when timing source loss simulated
- TEST-DARS-JITTER-001: Jitter metric accuracy under synthetic drift
- TEST-DARS-ERROR-001: Invalid sample rate triggers Error state
- TEST-DARS-TRP-001: TRP boundaries detected at correct frame indices

## 13. Open Issues

- Precise jitter window size parametrization (tunable?)
- Strategy for multi-rate operation (48k vs 96k) during continuous lock

## 14. Future Refactoring Opportunities

- Template-based sample rate specialization to remove runtime branching
- Optional SIMD normalization of TRP deltas if profiling indicates bottleneck

## 15. References

- ADR-002 (state machine), ADR-003 (timing abstraction), ADR-004 (memory), ADR-001 (layering)

