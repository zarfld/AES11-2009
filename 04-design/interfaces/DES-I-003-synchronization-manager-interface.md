# DES-I-003 Synchronization Manager Interface

ID: DES-I-003  
Title: ISynchronizationManager (hardware-agnostic)  
Spec Type: design (non-governed)  
Status: draft  
Owners: standards@project

## Purpose

Define the interface that coordinates timing sources, selects the master, manages degradation/holdover, and exposes synchronization status to clients. This interface is consumed by higher layers and implemented by DES-C-002 (Synchronization Manager) using DES-I-002 timing primitives.

## Responsibilities

- Accept multiple candidate timing sources with health/quality scores.
- Select and maintain an active master source; trigger reselection on degradation.
- Manage holdover and recovery transitions with bounded behavior.
- Expose synchronization status and active source identity to clients.

Out of scope:

- Direct hardware access; any OS/vendor specifics.

## Interface summary

Conceptual C++-like signature:

```cpp
struct TimingSourceInfo {
    std::string id;              // unique name
    uint32_t sample_rate_hz;     // AES5 compliant
    uint16_t quality_score;      // 0..1000 composite metric
    uint32_t capabilities;       // from DES-I-004
};

enum class SyncState { Acquiring, Locked, Holdover, Degraded };

struct SyncStatus {
    SyncState state;
    std::string active_source_id;
    uint16_t stability_ppm;
    uint32_t window_size;
};

class ISynchronizationManager {
public:
    virtual bool configure(uint32_t sample_rate_hz, uint32_t window_size) = 0; // REQ-F-SYNC-001
    virtual void register_source(const TimingSourceInfo& src) = 0;              // REQ-F-SYNC-001
    virtual void unregister_source(const std::string& id) = 0;                  // REQ-F-SYNC-004
    virtual bool update() = 0;                                                  // REQ-F-SYNC-002
    virtual SyncStatus get_status() const = 0;                                   // REQ-F-SYNC-001
    virtual std::string get_active_source() const = 0;                           // REQ-F-SYNC-003
    virtual ~ISynchronizationManager() = default;
};
```

## Pre/Post conditions

| Method | Pre-Conditions | Post-Conditions | Errors |
|---|---|---|---|
| configure | AES5 rate; window_size > 0 | Internal buffers prepared; state=Acquiring | false on invalid params |
| register_source | Unique id; AES5 rate match | Source becomes eligible for selection | N/A |
| update | At least one source registered | State may change; active source may update | false on inconsistent inputs |
| get_status | Configured | Returns latest snapshot | N/A |
| get_active_source | Configured | Returns id or empty if none | N/A |

## Performance & concurrency

- O(1) steady-state updates; O(N) only on reselection (N = number of sources) where N is small (<= 8).
- Lock-free read of status via snapshot.
- No dynamic allocation in hot path.

## Error codes (conceptual)

- ERR_INVALID_RATE, ERR_DUPLICATE_SOURCE, ERR_NO_SOURCES, ERR_INCONSISTENT_STATE

## Traceability

- Requirements: REQ-F-SYNC-001/002/003/004, REQ-NF-REL-001, REQ-NF-PERF-001
- Architecture: ARC-C-004 (Sync Manager)
- Design links: DES-C-002, DES-I-002, DES-I-004

## Notes & references

- Selection and holdover policies align with architectural decisions ADR-002 and ADR-003.
- Sample rates follow AES5; timing alignment targets AES-11 behavior without reproducing text.
