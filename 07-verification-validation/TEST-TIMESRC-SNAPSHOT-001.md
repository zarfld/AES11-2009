# TEST-TIMESRC-SNAPSHOT-001 Atomic Timing Snapshot: Monotonicity and Coherence

ID: TEST-TIMESRC-SNAPSHOT-001  
Spec Type: test (non-governed)  
Status: draft

## Purpose

Validate that `DES-I-002` provides an atomic `get_timing_snapshot()` with strict monotonic time and non-decreasing sample counter, and that the tuple `{time_ns, sample_counter, sample_rate_hz}` is coherent (no torn reads across fields).

## Scope

- Interface under test: DES-I-002 Timing Source Interface
- Collaborators: DES-D-002 (Timing models) and optional mock timing provider

## Preconditions

- AES5-compliant `sample_rate_hz` (e.g., 48000) configured and stable.
- Mock or platform provider capable of deterministic increments for time and sample counter.

## Test Steps

1. Call `get_timing_snapshot()` N times (N ≥ 1,000) in a tight loop; record sequence S[i].
2. For all i>0, assert `S[i].time_ns ≥ S[i-1].time_ns` (strictly monotonic; allow equality only if provider’s resolution is coarser than loop period; preferred strictly increasing).
3. For all i>0, assert `S[i].sample_counter ≥ S[i-1].sample_counter` and that the delta roughly matches `(S[i].time_ns - S[i-1].time_ns) * sample_rate_hz / 1e9` within acceptable bounds.
4. Compare snapshot fields against separate calls: take snapshot T = `get_timing_snapshot()`; immediately call `get_sample_time_ns()` and `get_sample_counter()` individually. Verify those may differ slightly from T, but T remains a self-consistent pair without cross-field skew.
5. Validate `sample_rate_hz ∈ AES5` and remains constant throughout the sequence.

## Acceptance Criteria

- No decrease in `time_ns` or `sample_counter` across the snapshot sequence.
- Snapshot internal coherence: derived interval based on fields is consistent within jitter bounds; no impossible combinations (e.g., time moved forward while counter moved backward).
- `sample_rate_hz` remains AES5-valid and constant for the duration of the test.

## Traceability

- Requirements: REQ-F-HAL-002, REQ-NF-PERF-001, REQ-NF-REL-001, REQ-NF-PORT-001
- Architecture: ARC-C-001, ARC-C-004; ADR-003
- Design: DES-I-002, DES-D-002, DES-C-003
