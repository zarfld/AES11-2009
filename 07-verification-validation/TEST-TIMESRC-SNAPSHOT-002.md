# TEST-TIMESRC-SNAPSHOT-002 Concurrency and TOCTOU Resilience

ID: TEST-TIMESRC-SNAPSHOT-002  
Spec Type: test (non-governed)  
Status: draft

## Purpose

Ensure `get_timing_snapshot()` remains atomic and coherent under concurrent access and that using separate getters (`get_sample_time_ns()`, `get_sample_counter()`) can exhibit TOCTOU skew, motivating the snapshot API usage.

## Scope

- Interface under test: DES-I-002 Timing Source Interface
- Collaborators: DES-D-002 (Timing models); concurrent worker threads

## Preconditions

- Multi-threaded test harness available (or simulated concurrency with interleaving hooks in mock provider).
- Stability thresholds and AES5-compliant rate configured.

## Test Steps

1. Start two threads:
   - T1: Calls `get_timing_snapshot()` in a loop (M ≥ 10k iterations), verifying per-iteration monotonicity and coherence.
   - T2: Simultaneously calls `get_sample_time_ns()` and `get_sample_counter()` separately in alternating order; records pairs.
2. Introduce controlled delays between separate calls in T2 to amplify TOCTOU effects.
3. Verify T1 never observes incoherent tuples; time and counter monotonic per thread ordering.
4. Demonstrate that some T2 pairs can be incoherent (e.g., time from t0 and counter from t1) beyond acceptable skew, documenting why snapshot must be preferred in hot paths.
5. Ensure no locks are taken in the hot path (validate performance budget qualitatively or via timers as guidance) and that the snapshot call remains O(1).

## Acceptance Criteria

- Under concurrency, snapshot results (T1) are always coherent and monotonic.
- Separate getters (T2) produce occasional skewed pairs under interleaving, illustrating TOCTOU risk; this is acceptable and documented.
- No deadlocks or contention spikes; steady-state calls remain O(1) with negligible jitter.

## Traceability

- Requirements: REQ-F-HAL-002, REQ-NF-PERF-001, REQ-NF-REL-001
- Architecture: ARC-C-004; ADR-003
- Design: DES-I-002, DES-D-002, DES-C-003
