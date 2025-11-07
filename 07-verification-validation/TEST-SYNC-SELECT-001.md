# TEST-SYNC-SELECT-001 Synchronization Source Selection: Priority and Stability

ID: TEST-SYNC-SELECT-001  
Spec Type: test (non-governed)  
Status: draft

## Purpose

Verify that the Synchronization Manager selects the highest-priority stable source and maintains selection stability with hysteresis to prevent flapping.

## Scope

- Component under test: DES-C-002 Synchronization Manager via DES-I-003
- Collaborators: DES-I-002 (Timing Source), DES-C-003 (Timing Window Processor), DES-D-002 (Timing models)

## Preconditions

- Two timing sources registered: A (priority=10), B (priority=20) where lower numeric value = higher priority.
- Stability thresholds: lock_threshold_ppm = 1, variance_threshold_ppm = 1, window_size ≥ 128.
- Source A metrics: mean_ppm ≈ 0.4, peak_ppm ≤ 1, variance_ppm ≤ 0.5 (stable).
- Source B metrics: mean_ppm ≈ 0.2, peak_ppm ≤ 1, variance_ppm ≤ 0.5 (stable).
- Hysteresis parameter K (consecutive stable evaluations required before switching) = 3.

## Test Steps

1. Initialize manager with policy: choose lowest priority among stable sources; tie-break on lower mean_ppm.
2. Feed stable windows for A and B until both report stability=true.
3. Call evaluate_selection(); expect selected_source == A (priority=10).
4. Inject jitter spikes into A (simulate peak_ppm > lock_threshold_ppm for ≥ window_size intervals) making A unstable.
5. Call evaluate_selection(); expect selected_source transitions to B within one cycle.
6. Restore A stability but keep a counter of stable evaluations; before K cycles elapse verify selection remains B.
7. After K consecutive stable evaluations for A, call evaluate_selection(); expect selected_source switches back to A.
8. Ensure no selection oscillation occurred without meeting hysteresis condition.

## Acceptance Criteria

- Selection favors lowest priority value when multiple stable sources exist.
- Transition away from a degraded source occurs within one evaluation cycle.
- Re-selection back to recovered source only after K consecutive stable evaluations.
- No flapping (more than one switch within < K stability cycles) observed.

## Traceability

- Requirements: REQ-F-SYNC-001, REQ-F-SYNC-002, REQ-NF-REL-001, REQ-NF-PERF-001
- Architecture: ARC-C-004; ADR-001, ADR-003
- Design: DES-C-002, DES-I-003, DES-I-002, DES-C-003, DES-D-002
