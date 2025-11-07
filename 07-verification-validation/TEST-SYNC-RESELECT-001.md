# TEST-SYNC-RESELECT-001 Forced Reselection Chooses Next Viable Source

ID: TEST-SYNC-RESELECT-001  
Spec Type: test (non-governed)  
Status: draft

## Purpose

Validate that a forced reselection (admin or automatic trigger) selects the next best stable source according to priority and stability ranking, skipping degraded or unstable candidates.

## Scope

- Component under test: DES-C-002 (Synchronization Manager)
- Collaborators: DES-I-003 interface, DES-I-002 timing sources, DES-C-003 window processor

## Preconditions

- Current active source S0 stable, priority=10.
- Additional sources: S1 priority=15 (stable), S2 priority=20 (degraded), S3 priority=25 (stable, lower mean_ppm than S1).
- Selection policy: lowest priority value first; if forced reselection, exclude current S0 and pick next stable lowest priority.

## Test Steps

1. Register S0..S3 and achieve stability statuses as defined.
2. Confirm baseline selection = S0.
3. Issue forced_reselection() command.
4. Verify chosen source = S1 (next lowest priority stable, excluding S0).
5. Mark S1 unstable (inject variance_ppm spike > threshold); issue forced_reselection() again.
6. Verify chosen source = S3 (skip degraded S2, unstable S1).
7. If all alternates unstable/degraded, forced reselection retains S0 and logs reason code.

## Acceptance Criteria

- Forced reselection excludes current source and any degraded/unstable sources.
- Selection order honors priority before secondary metrics (mean_ppm) unless priorities equal.
- Retention behavior documented when no viable alternative exists.

## Traceability

- Requirements: REQ-F-SYNC-002, REQ-F-SYNC-003, REQ-NF-REL-001
- Architecture: ARC-C-004; ADR-001, ADR-003
- Design: DES-C-002, DES-I-003, DES-I-002, DES-C-003, DES-D-002
