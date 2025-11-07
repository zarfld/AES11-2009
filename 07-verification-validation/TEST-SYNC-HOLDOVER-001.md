# TEST-SYNC-HOLDOVER-001 Holdover Drift Within Grade Accuracy Bounds

ID: TEST-SYNC-HOLDOVER-001  
Spec Type: test (non-governed)  
Status: draft

## Purpose

Verify that during holdover mode (loss of external reference), accumulated drift remains within configured grade accuracy (±1 ppm for Grade 1, ±10 ppm for Grade 2) over evaluation interval T, and that corrective smoothing applies using last stable window metrics.

## Scope

- Component under test: DES-C-002 (Synchronization Manager) holdover logic
- Collaborators: DES-C-003 (Timing Window Processor), DES-I-002 (Timing Source), DES-D-002 (Timing models)

## Preconditions

- Source S0 previously locked (Grade 1) with mean_ppm ≈ 0.3, variance_ppm ≤ 0.5.
- External reference loss event simulated (e.g., no new TRP updates for N frames where N > timeout_threshold).
- Holdover parameters: grade=1, max_drift_ppm = 1 ppm, evaluation interval T = 10 seconds simulated via frame timestamps.

## Test Steps

1. Establish locked state on S0; snapshot baseline timestamp t0.
2. Trigger external reference loss; enter holdover state.
3. Simulate passage of T seconds via frame progression and internal clock increments; compute drift_ppm from timing reference model.
4. Verify drift_ppm ≤ max_drift_ppm (1 ppm) at end of interval.
5. Introduce synthetic local oscillator error (+0.5 ppm) mid-holdover; verify smoothing uses prior window mean to limit effective drift.
6. Restore external reference; verify transition back to locked state with updated baseline and no discontinuity (> 1 frame interval jump) in reported timing reference.

## Acceptance Criteria

- Drift stays within ±1 ppm over interval T for Grade 1 scenario.
- Smoothing reduces oscillator error impact (effective drift < raw injected drift after correction step).
- Re-lock occurs smoothly with monotonic reference timestamp.

## Traceability

- Requirements: REQ-F-SYNC-004, REQ-F-DARS-003, REQ-NF-REL-001
- Architecture: ARC-C-004; ADR-002, ADR-003
- Design: DES-C-002, DES-C-003, DES-I-003, DES-I-002, DES-D-002
