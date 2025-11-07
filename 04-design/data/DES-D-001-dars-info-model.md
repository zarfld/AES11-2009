---
title: "Software Design Description - DARS Info Model"
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
  - REQ-NF-PERF-001
  - REQ-NF-PORT-001
relatedADRs:
  - ADR-001
  - ADR-002
  - ADR-003
  - ADR-004
traceabilityTier: data-model
---
## DES-D-001 DARS Info Model

## 1. Purpose and Scope
 
Represents protocol-facing DARS information exchanged between components in the standards layer. This model focuses on encapsulating timing reference points (TRP) and related metrics as Value Objects, ensuring immutability and type safety while minimizing coupling to concrete implementations.

## 2. Classification and Design Principles

- Classification: Value Objects aggregated by an Entity "SyncSession" (see DES-C-002 for owning component).
- Principles applied: Program to interfaces, favor composition, law of Demeter, Tell-Don't-Ask, DRY.
- Immutability: Value objects are immutable; updates occur via replacement.

## 3. Responsibilities

- Provide a type-safe representation of timing references and jitter metrics.
- Capture minimal but sufficient fields to compute and validate synchronization status.
- Enable deterministic serialization for testing and logging (no persistence in standards layer).

## 4. Data Types and Structures

### 4.1 `TimingReference`

Immutable value representing a TRP and related derived quantities.

```cpp
struct TimingReference {
    uint64_t timestamp_ns;    // High-resolution monotonic time at TRP
    uint64_t frame_index;     // Sequential frame index at TRP boundary
    uint32_t sample_rate_hz;  // One of AES5 preferred sampling rates
};
```

Invariants:

- `sample_rate_hz` must be a valid AES5 constant.
- `timestamp_ns` monotonic (non-decreasing across sequence snapshots).

### 4.2 `JitterMetrics`

Aggregate statistics for recent TRP intervals.

```cpp
struct JitterMetrics {
    uint32_t peak_ppm;        // Peak deviation over window (parts per million)
    uint32_t mean_ppm;        // Mean absolute deviation over window (ppm)
    uint16_t window_samples;  // Sample count used for statistics
};
```

Invariants:

- `window_samples` > 0 when `peak_ppm` or `mean_ppm` is non-zero.

### 4.3 `LockStatus`

```cpp
enum class LockState : uint8_t { Acquiring, Locked, Holdover, Error };

struct LockStatus {
    LockState state;
    uint16_t stability_ppm;     // Current stability metric; lower is better
    bool degraded;              // True when below performance thresholds
};
```

Invariants:

- `stability_ppm` is bounded by configured capture/holdover ranges.

### 4.4 `DARSInfo` (Aggregate Value Snapshot)

```cpp
struct DARSInfo {
    TimingReference trp;     // Timing reference point snapshot
    JitterMetrics   jitter;  // Recent jitter statistics
    LockStatus      lock;    // Lock/holdover status
};
```

Design choice: `DARSInfo` is a snapshot object to be passed between components; mutation occurs by creating a new snapshot instance with updated fields.

## 5. Associations and Aggregates

- Aggregate root: Synchronization Manager (DES-C-002) acts as the controlling Entity for lifecycle, exposing `DARSInfo` snapshots.
- External references: AES3/AES5 libraries provide parsing and constants; not embedded in data model.
- No bidirectional associations between value objects.

## 6. Construction and Factories

- Construction: use explicit constructors or factory helpers to validate invariants.

```cpp
struct DARSInfoFactory {
    static DARSInfo make(const TimingReference& trp,
                         const JitterMetrics& jitter,
                         const LockStatus& lock);
};
```

Factory responsibilities:

- Validate `sample_rate_hz` against AES5 constants.
- Ensure monotonicity of TRP timestamps when compared to previous snapshot (if provided by caller in higher-level factory overloads).

## 7. Equality, Hashing, and Ordering

- `TimingReference`, `JitterMetrics`, `LockStatus` implement value semantics (equality by all fields).
- `DARSInfo` equality compares all contained fields.
- Optional ordering helpers for testing can compare by `trp.frame_index` then `trp.timestamp_ns`.

## 8. Serialization for Testing

- Provide deterministic string/JSON-like format in tests (outside standards layer). The standards layer exposes minimal `to_struct()`/accessors only.

## 9. Error Handling

- Validation failures reported via factory return status (bool/expected<T,Err>) or by separate error code returned alongside object construction.
- No exceptions in hot paths.

## 10. Security and Integrity

- Validate all inputs; guard against integer overflow in derived computations (e.g., ppm calculations use 64-bit intermediates).

## 11. Traceability

| Requirement | Mechanism |
|-------------|-----------|
| REQ-F-DARS-001 | `LockStatus` and invariants for acquiring/locked states |
| REQ-F-DARS-002 | `stability_ppm` and state transitions by owner component |
| REQ-F-DARS-003 | `TimingReference` snapshot and monotonic constraint |
| REQ-NF-PERF-001 | `JitterMetrics` fields and window semantics |
| REQ-NF-PORT-001 | Model free of vendor/OS types; pure POD-style structs |

## 12. Tests (Planned)

- TEST-DM-DARSINFO-001: Factory rejects invalid sample rate.
- TEST-DM-DARSINFO-002: TRP monotonicity enforced.
- TEST-DM-DARSINFO-003: Jitter window semantics respected.
- TEST-DM-DARSINFO-004: Value equality and hashing behave as expected.
