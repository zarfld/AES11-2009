# TEST-SYNC-DEGRADE-001 Degradation Detection on Sustained Jitter Spike

ID: TEST-SYNC-DEGRADE-001  
Spec Type: test (non-governed)  
Status: draft

## Purpose

Validate that the Synchronization Manager marks the active source as degraded when peak_ppm exceeds configured threshold for M consecutive window updates, and triggers downstream adaptation (e.g., increased monitoring frequency) without immediate source reselection unless stability fully lost.

## Scope

- Component under test: DES-C-002 (Synchronization Manager)
- Collaborators: DES-C-003 (Timing Window Processor), DES-I-002 (Timing Source), DES-D-002 (Timing models)

## Preconditions

- Active source S0 locked with initial metrics: mean_ppm ≈ 0, peak_ppm ≤ 1.
- Degradation threshold: degrade_peak_threshold_ppm = 5.
- Sustained window count M = 4 (number of consecutive windows exceeding threshold to assert degradation).
- Reselection policy: do not switch unless stability flag clears or alternate source superior & stable.

## Test Steps

1. Achieve locked state on S0 (use prior TEST-DARS-STATE pattern).
2. Feed intervals causing peak_ppm ≈ 6 (> threshold) for 1 ≤ i < M windows; verify degradation flag remains false until i == M.
3. On M-th consecutive degraded window, verify degradation flag true and manager raises a degradation event.
4. Verify selection does not flip if alternate sources either absent or not strictly better (simulate one alternate with mean_ppm ≈ 0.8, peak_ppm 3).
5. Feed recovery windows (peak_ppm ≤ 2) for R consecutive windows where R = clear_degrade_hysteresis (e.g., 3); ensure flag clears only after R.
6. Confirm monitoring frequency (if part of design) adjusted during degradation and restored after recovery.

## Acceptance Criteria

- Degradation asserted only after M consecutive threshold breaches.
- No false degradation on transient single-window spike.
- Degradation clears only after R consecutive recovery windows.
- Selection remains stable unless stability is lost or better source available per policy.

## Traceability

- Requirements: REQ-F-SYNC-003, REQ-NF-REL-001, REQ-NF-PERF-001
- Architecture: ARC-C-004; ADR-002, ADR-003
- Design: DES-C-002, DES-C-003, DES-I-003, DES-I-002, DES-D-002
