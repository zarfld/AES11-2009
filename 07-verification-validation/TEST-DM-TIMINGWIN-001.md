# TEST-DM-TIMINGWIN-001 Timing Window Mean/Peak/Variance

ID: TEST-DM-TIMINGWIN-001  
Spec Type: test (non-governed)  
Status: draft

## Purpose

Validate that the Timing Window Processor (DES-C-003) computes mean_ppm, peak_ppm, and variance_ppm correctly and updates in O(1) as new intervals are ingested.

## Scope

- Component under test: DES-C-003 Timing Window Processor
- Collaborators: DES-D-002 (TimingWindow, IntervalError), DES-I-002 (for snapshot generation in integrated runs)

## Preconditions

- window_size = 8, lock_threshold_ppm = 1, holdover_threshold_ppm = 10
- nominal_ns = 20833.333… ns for 48 kHz frame interval (approximate acceptable)

## Test Steps

1. Configure processor with window_size=8.
2. Ingest 8 intervals with measured_ns = nominal_ns (expect ppm_error ≈ 0 for all).
3. Verify mean_ppm ≈ 0, peak_ppm ≈ 0, variance_ppm ≈ 0; lock_suggest=true.
4. Ingest one outlier with +5 ppm; verify peak_ppm ≈ 5, mean_ppm shifts slightly, variance_ppm increases.
5. Continue with stable intervals; verify peak decays when outlier falls out of window, variance reduces.

## Acceptance Criteria

- Aggregates update without reprocessing entire window (observable by stable runtime and correct values).
- Lock suggestion aligns to thresholds when stable; clears appropriately after persistent outliers.

## Traceability

- Requirements: REQ-F-DARS-003, REQ-F-SYNC-001, REQ-NF-PERF-001
- Architecture: ARC-C-004, ADR-003
- Design: DES-C-003, DES-D-002, DES-I-002
