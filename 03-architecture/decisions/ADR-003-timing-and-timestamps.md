---
specType: guidance
id: ADR-003
title: ADR-003 Timing and Timestamp Sources Abstraction
author: "AI Assistant"
date: "2025-11-07"
---
## ADR-003: Timing and Timestamp Sources Abstraction

## Status

Accepted

## Context

Precise timing is required for TRP alignment, jitter measurement, and synchronization. Platforms provide different timing sources and resolutions.

## Decision

Abstract timing via timing_hal_t providing:

- Monotonic nanosecond timestamps (no backward jumps)
- Sample-accurate counters at configured sample rate
- Resolution query for validation

Fallbacks provided when hardware timestamps unavailable.

## Consequences

### Positive

- Deterministic timing independent of platform
- Enables testing and simulation

### Negative

- Requires careful calibration on some platforms

## Alternatives Considered

- Direct OS APIs in standards layer – rejected (portability violation)

## Compliance and Standards

- Requirements: REQ-F-HAL-002, REQ-NF-PERF-001..003, REQ-F-DARS-004

## Implementation Notes

- Use capability discovery to select optimal paths (HW timestamp vs software)
- Validate monotonicity and resolution at startup

## Review and Approval

- Proposed by: Architecture Team
- Approved by: Technical Lead
- Date: 2025-11-07
