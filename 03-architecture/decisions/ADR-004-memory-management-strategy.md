---
specType: guidance
id: ADR-004
title: ADR-004 Memory Management Strategy (Real-Time Safe)
author: "AI Assistant"
date: "2025-11-07"
---
## ADR-004: Memory Management Strategy (Real-Time Safe)

## Status

Accepted

## Context

Embedded targets require static allocation and predictable behavior; desktop targets can use dynamic allocation outside real-time paths.

## Decision

- Real-time paths: static/pool allocation only; no malloc/free
- Non-real-time: dynamic allocation permitted with tracking
- Provide memory_hal_t with configurable backends

## Consequences

### Positive

- Deterministic behavior on embedded targets
- Cross-platform consistency

### Negative

- Slightly higher code complexity

## Alternatives Considered

- Allow dynamic allocation in RT paths – rejected (non-deterministic)

## Compliance and Standards

- Requirements: REQ-F-HAL-005, REQ-NF-PERF-004, StR-PERF-005

## Implementation Notes

- Add allocation instrumentation hooks for tests
- Provide compile-time options for embedded profiles

## Review and Approval

- Proposed by: Architecture Team
- Approved by: Technical Lead
- Date: 2025-11-07
