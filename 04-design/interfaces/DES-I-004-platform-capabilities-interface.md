# DES-I-004 Platform Capabilities Interface

ID: DES-I-004  
Title: IPlatformCapabilities (timestamp & discipline features)  
Spec Type: design (non-governed)  
Status: draft  
Owners: standards@project

## Purpose

Provide an abstraction for optional platform timing capabilities (hardware timestamping, PPS, PTP correlation) so synchronization logic (DES-C-002) can adapt behavior without embedding hardware specifics.

## Responsibilities

- Report availability of fine-grained hardware timestamps.
- Indicate presence of PPS (pulse-per-second) discipline signal.
- Indicate whether system clock is correlated to an external PTP (IEEE 1588) grandmaster.

Out of scope:

- Implementing discipline algorithms; this interface only advertises capability presence.

## Interface summary

```cpp
enum class CapabilityFlag : uint32_t {
    HardwareTimestamp = 0x01,   // REQ-NF-PORT-001
    PPSDiscipline      = 0x02,  // REQ-NF-REL-001
    PTPCorrelated      = 0x04   // REQ-F-SYNC-003 (indirect support)
};

class IPlatformCapabilities {
public:
    virtual uint32_t get_capabilities_mask() const = 0;               // bitwise OR of flags
    virtual bool has(CapabilityFlag flag) const = 0;                   // convenience helper
    virtual uint64_t get_hw_timestamp_resolution_ns() const = 0;       // 0 if unsupported
    virtual ~IPlatformCapabilities() = default;
};
```

## Usage pattern

- Injected into DES-C-002; influences jitter window sizing and holdover thresholds.
- Queried by DES-I-003 implementer when evaluating source quality.

## Traceability

- Requirements: REQ-NF-PORT-001, REQ-NF-REL-001, REQ-F-SYNC-003
- Architecture: ARC-C-004 (Sync Manager), ARC-C-001 layering
- Design links: DES-C-002, DES-I-002, DES-I-003

## Performance

- All getters O(1); no allocations.
- Mask computed at initialization; queries are simple bit tests.

## Error handling

- Absent capabilities simply return false / 0; no error codes required.

## References

- Aligns with ADR-003 timing considerations; external standards IEEE 1588 referenced conceptually without reproduction.
