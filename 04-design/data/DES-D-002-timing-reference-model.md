---
title: "Software Design Description - Timing Reference Model"
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
  - REQ-F-DARS-003
  - REQ-NF-PERF-001
  - REQ-NF-REL-001
relatedADRs:
  - ADR-003
  - ADR-004
traceabilityTier: data-model
---
## DES-D-002 Timing Reference Model

## 1. Purpose

Defines Value Objects and an Aggregate boundary for capturing timing reference deltas and quality information used by synchronization and holdover strategies.

## 2. Model Overview

- Value Objects: `NominalInterval`, `MeasuredInterval`, `IntervalError`.
- Aggregate Root: `TimingWindow` (encapsulates a fixed-size window of intervals and computes quality metrics).
- Factory: `TimingWindowFactory` for safe construction and resizing.

## 3. Types and Invariants

### 3.1 `NominalInterval`

```cpp
struct NominalInterval {   // immutable value
    uint64_t ns;           // nominal interval in nanoseconds
};
```

Invariant: `ns > 0` and derived from `sample_rate_hz`.

### 3.2 `MeasuredInterval`

```cpp
struct MeasuredInterval {  // immutable value
    uint64_t ns;           // measured TRP-to-TRP interval
    uint64_t at_index;     // frame index at end of interval
};
```

Invariant: `ns > 0` and `at_index` monotonic.

### 3.3 `IntervalError`

```cpp
struct IntervalError {     // immutable value
    int32_t ppm;           // signed deviation from nominal in ppm
};
```

Invariant: computed using 64-bit intermediates to avoid overflow.

### 3.4 `TimingWindow` (Aggregate Root)

```cpp
struct TimingWindow {
    NominalInterval nominal;   // nominal expected interval
    uint16_t capacity;         // fixed-size ring buffer capacity
    // opaque storage managed internally (design-time spec, impl detail hidden)
};
```

Responsibilities:

- Accept `MeasuredInterval` samples and produce `IntervalError` values.
- Maintain rolling statistics: peak/mean ppm and stability flags.
- Provide snapshot method to export `JitterMetrics` (DES-D-001).

Invariants:

- `capacity >= 8` (minimum window size for meaningful stats).
- All inserts are O(1) and non-allocating after construction.

## 4. Algorithms

- PPM error: `ppm = (measured - nominal) * 1'000'000 / nominal` using 128-bit intermediate if available, else guarded 64-bit.
- Peak/mean computed over current window; stability flag set if peak < configured threshold.

## 5. Factories

```cpp
struct TimingWindowFactory {
    static TimingWindow make(NominalInterval nominal, uint16_t capacity);
};
```

Factory checks:

- `nominal.ns` derived from AES5-approved sample rate.
- `capacity` within bounded range (e.g., [8, 4096]).

## 6. Aggregate Rules

- Only `TimingWindow` is referenced externally; internal storage/types are encapsulated.
- No exposure of internal containers; use methods to add samples and get stats.

## 7. Error Handling

- Invalid inputs return status/error without throwing in hot paths.
- Saturate ppm computation on extreme inputs; report via status.

## 8. Security & Integrity

- Validate input ranges; defend against wrap-around and time regressions.

## 9. Traceability

| Requirement | Mechanism |
|-------------|-----------|
| REQ-F-SYNC-001 | Stability flags and rolling quality metrics |
| REQ-F-DARS-003 | Interval derivation from TRP deltas |
| REQ-NF-PERF-001 | O(1) updates, fixed-size window, overflow-safe math |
| REQ-NF-REL-001 | Resilience via saturation and window capacity bounds |

## 10. Tests (Planned)

- TEST-DM-TIMINGWIN-001: Factory bounds and nominal derivation.
- TEST-DM-TIMINGWIN-002: PPM calculation correctness and saturation.
- TEST-DM-TIMINGWIN-003: Stability flag behavior under jitter scenarios.
