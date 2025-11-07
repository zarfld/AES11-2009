# TEST-DARS-STATE-001 DARS State Transitions: Acquire → Lock

ID: TEST-DARS-STATE-001  
Spec Type: test (non-governed)  
Status: draft

## Purpose

Verify that the DARS Protocol Core transitions from Acquiring to Locked when provided a valid sequence of timing reference points (TRPs) and stable jitter metrics within configured thresholds.

## Scope

- Component under test: DES-C-001 (DARS Protocol Core) via DES-I-001 interface
- Collaborators: DES-I-002 (Timing Source), DES-C-003 (Timing Window Processor), DES-D-001/002 data models

## Preconditions

- Configuration: sample_rate_hz ∈ AES5, lock_threshold_ppm = 1, jitter_window = 128
- Initial state: Acquiring
- Valid frames with correctly detected TRP boundaries are provided consecutively

## Test Steps

1. Initialize IDARSProtocolCore with AES5-compliant sample rate and thresholds.
2. Feed N frames (N ≥ jitter_window) with TRP boundaries at expected cadence.
3. Ensure jitter window inputs produce mean_ppm ≈ 0 and peak_ppm ≤ 2 ppm.
4. Query get_lock_status(); expect Locked.
5. Query get_timing_reference(); expect non-zero, monotonic TRP timestamp.

## Acceptance Criteria

- State becomes Locked within N frames after stable input begins.
- Reported TRP is non-zero and increases monotonically across successive calls.

## Traceability

- Requirements: REQ-F-DARS-001, REQ-F-DARS-002, REQ-F-DARS-003, REQ-NF-PERF-001
- Architecture: ARC-C-001, ARC-C-004; ADR-002, ADR-003
- Design: DES-C-001, DES-I-001, DES-I-002, DES-C-003, DES-D-001, DES-D-002
