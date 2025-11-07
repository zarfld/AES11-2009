---
specType: guidance
id: ADR-001
title: ADR-001 Layered Architecture with HAL Boundaries
author: "AI Assistant"
date: "2025-11-07"
---
## ADR-001: Layered Architecture with HAL Boundaries

## Status

Accepted

## Context

The standards layer must remain hardware- and vendor-agnostic while supporting embedded and desktop platforms. All hardware access must be abstracted via HAL interfaces to keep AES::AES11::_2009 free from OS and vendor dependencies.

## Decision

Use a layered architecture:

- Standards Layer: AES::AES11::_2009 (core/dars/sync/video/conformity)
- Service Layer: Platform HAL implementations (ASIO/ALSA/CoreAudio/RTOS)
- External Standards: AES3-2009, AES5-2018 reused as separate repos

Interfaces between layers are defined as C function-pointer structs (audio_hal_t, timing_hal_t, sync_hal_t) to ensure zero-overhead and maximal portability.

## Consequences

### Positive

- Maintains strict portability and testability
- Enables multi-platform support without forking core
- Simplifies conformance validation

### Negative

- Requires careful HAL design to avoid leakage or missing capabilities
- Slight indirection overhead (mitigated by inlining/optimization)

### Neutral

- Clear separation of concerns increases initial design work

## Alternatives Considered

- Direct hardware calls inside standards layer – rejected (violates portability)
- C++ virtual interfaces – rejected for embedded overhead and ABI issues

## Compliance and Standards

- IEEE 42010: Clear separation of concerns and rationale
- Requirements: REQ-F-HAL-001..007, REQ-NF-PORT-001..003

## Implementation Notes

- Provide mock HAL for tests
- Provide validation tool for third-party HALs
- Maintain namespace isolation per repository guidelines

## Review and Approval

- Proposed by: Architecture Team
- Approved by: Technical Lead
- Date: 2025-11-07
