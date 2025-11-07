# DES-C-003 Timing Window Processor Component

ID: DES-C-003  
Title: Timing Window Processor  
Spec Type: design (non-governed)  
Status: draft  
Owners: standards@project

## 1. Purpose

Implements windowed timing interval aggregation and stability metrics used for jitter evaluation, lock qualification, and holdover decision support. Consumes immutable data models (DES-D-002 TimingWindow, IntervalError) and timing snapshots from DES-I-002. Supplies processed metrics to DES-C-002 (Synchronization Manager) and DES-C-001 (DARS Protocol Core) without duplicating their logic.

## 2. Responsibilities

- Maintain a bounded circular buffer of measured intervals (size = configured window_size).
- Compute derived ppm error, mean, peak, and stability variance in O(1) amortized time.
- Provide threshold evaluations (lock, holdover entry/exit) using configuration constants.
- Expose snapshot metrics atomically for concurrent readers.

Out of scope:

- Source selection (handled by DES-C-002).
- TRP boundary detection (handled by DES-C-001).

## 3. Interfaces & Collaborators

- Depends on DES-I-002 (Timing Source) for snapshots.
- Uses DES-D-002 data models for interval representation.
- Provides metrics struct consumed by ISynchronizationManager (DES-I-003) implementation.

Conceptual interface:

```cpp
struct WindowMetrics {
    uint64_t samples_in_window;    // total samples covered
    uint16_t window_size;          // configured size
    double mean_ppm;               // average ppm error
    double peak_ppm;               // max absolute ppm deviation
    double variance_ppm;           // statistical variance
    bool lock_suggest;             // threshold satisfied for lock
    bool holdover_suggest;         // threshold for holdover stability
};

class ITimingWindowProcessor {
public:
    virtual bool configure(uint16_t window_size, uint16_t lock_threshold_ppm,
                           uint16_t holdover_threshold_ppm) = 0; // REQ-F-SYNC-001
    virtual bool ingest_interval(uint64_t nominal_ns, uint64_t measured_ns) = 0; // REQ-F-DARS-003
    virtual WindowMetrics get_metrics() const = 0;                                // REQ-NF-PERF-001
    virtual void reset() = 0;                                                    // REQ-F-ERROR-001
    virtual ~ITimingWindowProcessor() = default;
};
```

## 4. Data Structures

Internal representation (optimized for real-time):

- Fixed-size array (window_size) of int32 ppm errors (scaled by 1e6).
- Running aggregates: sum_ppm, peak_ppm, sum_sq_ppm for variance.
- Atomic snapshot struct for readers (copy-on-write pattern).

## 5. Algorithms

Interval ingestion:

1. Compute ppm_error = ((measured_ns - nominal_ns) * 1e6) / nominal_ns.
2. Insert into circular slot; subtract previous value from aggregates; add new value.
3. Update peak if |ppm_error| > current peak.
4. Recompute variance using Welford incremental method (constant time).
5. Evaluate thresholds:
    - lock_suggest: |mean_ppm| <= lock_threshold_ppm AND peak_ppm <= 2 * lock_threshold_ppm.
    - holdover_suggest: variance_ppm <= (holdover_threshold_ppm * 0.25).

Complexity: O(1) per ingest with fixed window.

## 6. Concurrency

- Single writer (ingest_interval) + multiple readers (get_metrics).
- Metrics snapshot updated after each ingest; readers get consistent view without locks.

## 7. Performance

- No dynamic allocation after configure.
- Target ingest_interval execution budget < 10 µs (contributes to overall < 50 µs processing pipeline; REQ-NF-PERF-001).

## 8. Error Handling

- ingest_interval returns false if window not configured or nominal_ns == 0.
- configure returns false on invalid thresholds or window_size == 0.
- reset clears aggregates and sets lock_suggest/holdover_suggest false.

## 9. Traceability

| Requirement | Mechanism |
|-------------|-----------|
| REQ-F-DARS-003 | Ingestion derives timing reference stability metrics |
| REQ-F-SYNC-001 | Configuration & window-based stability evaluation |
| REQ-NF-PERF-001 | O(1) ingest, bounded execution budget |
| REQ-NF-REL-001 | Stability metrics inform reliable source selection |
| REQ-F-ERROR-001 | reset path for recovery |

Architecture references: ARC-C-004 (Sync Manager), ADR-002 (state machine timing impacts), ADR-003 (timestamp strategy).

Design references: DES-C-002 (consumption), DES-D-002 (data model), DES-I-002 (timing source).

## 10. Planned Tests

- TEST-DM-TIMINGWIN-001: Mean ppm converges with stable intervals.
- TEST-DM-TIMINGWIN-002: Peak ppm updates on outlier.
- TEST-DM-TIMINGWIN-003: Variance decreases under stable input.
- TEST-SYNC-DEGRADE-001: Degradation flag triggers when peak exceeds threshold.

## 11. Open Issues

- Potential adaptive window sizing for rapid lock; deferred (YAGNI).
- Consider exposing median_ppm (would require additional storage/search; currently omitted for simplicity).

## 12. References

- AES-11-2009 sections on timing stability (referenced implicitly; no text reproduced).
- AES5-2018 sampling rates for nominal interval derivation.
