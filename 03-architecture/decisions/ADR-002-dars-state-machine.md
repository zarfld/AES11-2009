---
specType: guidance
id: ADR-002
title: ADR-002 DARS State Machine (Acquire/Lock/Holdover)
author: "AI Assistant"
date: "2025-11-07"
---
## ADR-002: DARS State Machine (Acquire/Lock/Holdover)

## Status

Accepted

## Context

DARS synchronization requires robust acquisition, lock detection, and holdover behavior with Grade 1/2 tolerances.

## Decision

Implement a deterministic state machine with states: INIT → ACQUIRE → LOCKED → HOLDOVER → (recover to ACQUIRE). Transitions driven by:

- TRP detection quality and continuity
- Frequency error within capture range (Grade-dependent)
- Loss-of-signal/timeouts transitioning to HOLDOVER

## Consequences

### Positive

- Predictable synchronization behavior
- Clear testing and conformance mapping to AES-11 sections

### Negative

- Increased complexity in edge cases (e.g., jitter bursts)

## Alternatives Considered

- Ad-hoc flags without formal states – rejected (hard to validate)

## Compliance and Standards

- Requirements: REQ-F-SYNC-001..004, REQ-F-DARS-003/004, REQ-NF-REL-003

## Implementation Notes

- Capture range thresholds parameterized by Grade
- Expose internal state for diagnostics/testing
- Provide timeouts and hysteresis to avoid flapping

## Review and Approval

- Proposed by: Architecture Team
- Approved by: Technical Lead
- Date: 2025-11-07
