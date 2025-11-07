---
title: "Software Design Description - IDARSProtocolCore Interface"
specType: design
version: 0.1.0
status: draft
author: "AI Assistant"
date: "2025-11-07"
relatedArchitecture:
  - ARC-C-001
  - ARC-C-002
relatedRequirements:
  - REQ-F-DARS-001
  - REQ-F-DARS-002
  - REQ-F-DARS-003
  - REQ-F-SYNC-001
  - REQ-NF-PORT-001
relatedADRs:
  - ADR-001
  - ADR-002
  - ADR-004
traceabilityTier: interface
---
## DES-I-001 IDARSProtocolCore Interface

## 1. Purpose

Defines the protocol-facing contract for interacting with the DARS core component (DES-C-001) in a hardware-agnostic manner.

## 2. Interface Summary

```cpp
class IDARSProtocolCore {
public:
    virtual bool initialize(const DARSConfig& cfg) = 0;          // REQ-F-DARS-001
    virtual bool update(const FrameContext& frameCtx) = 0;       // REQ-F-DARS-002
    virtual DARSState get_lock_status() const = 0;                // REQ-F-DARS-001
    virtual uint64_t get_timing_reference() const = 0;            // REQ-F-DARS-003
    virtual JitterMetrics get_jitter_metrics() const = 0;         // REQ-NF-PERF-001
    virtual void reset() = 0;                                    // Support error recovery
    virtual ~IDARSProtocolCore() = default;                       // Polymorphic cleanup
};
```

## 3. Configuration Structure

```cpp
struct DARSConfig {
    uint32_t sample_rate_hz;      // Must match AES5 constants
    uint16_t lock_threshold_ppm;  // Stability threshold for lock
    uint16_t holdover_threshold_ppm; // Threshold to remain in holdover
    uint16_t jitter_window;       // Number of intervals to average
};
```

## 4. Pre/Post Conditions

| Method | Pre-Conditions | Post-Conditions | Errors |
|--------|----------------|-----------------|--------|
| initialize | `sample_rate_hz` valid; not yet initialized | Internal buffers allocated; state=Acquiring | false if invalid rate |
| update | Initialized; frameCtx fields valid | State transitions may occur; metrics updated | false on protocol violation |
| get_lock_status | Initialized | Returns current state | N/A |
| get_timing_reference | Locked or Holdover preferred | Returns last TRP (may be predictive in Holdover) | 0 if acquiring |
| get_jitter_metrics | Initialized | Returns accumulated metrics | Defaults if insufficient samples |
| reset | Any state | State=Acquiring; buffers cleared | N/A |

## 5. Error Codes (Conceptual)

- CONFIG_INVALID_SAMPLE_RATE
- FRAME_OUT_OF_RANGE
- TRP_SEQUENCE_ERROR
- JITTER_WINDOW_UNDERRUN

## 6. Threading & Concurrency

- Expected single producer (frame update) + multiple readers (status queries).
- Design choice: internal lock-free snapshot for jitter metrics using copy-on-write small struct.

## 7. Performance Considerations

- All getters inline and O(1).
- `update()` avoids memory allocation and heavy arithmetic.

## 8. Security & Validation

- Validate sample rate using external AES5 constants (no hardcoded values).
- FrameContext `is_trp_boundary` must be consistent with AES3 preamble detection.

## 9. Traceability

| Requirement | Mechanism |
|-------------|-----------|
| REQ-F-DARS-001 | initialize + get_lock_status contract |
| REQ-F-DARS-002 | update processing path |
| REQ-F-DARS-003 | get_timing_reference TRP exposure |
| REQ-F-SYNC-001 | Stable timing reference retrieval |
| REQ-NF-PORT-001 | Hardware-agnostic abstractions |

## 10. Planned Tests

- TEST-DARS-IFACE-INIT-001: Reject invalid sample rate.
- TEST-DARS-IFACE-UPDATE-001: State transition after valid TRP sequence.
- TEST-DARS-IFACE-JITTER-001: Jitter window metrics computed correctly.

## 11. Open Issues

- Whether to expose lock stability metric directly vs via jitter metrics only.

## 12. References

- ADR-001 (layering), ADR-002 (state machine), ADR-004 (memory strategy).
