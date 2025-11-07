---
specType: guidance
id: ADR-005
title: ADR-005 External Standards Integration (AES3/AES5)
author: "AI Assistant"
date: "2025-11-07"
---
## ADR-005: External Standards Integration (AES3/AES5)

## Status

Accepted

## Context

DARS uses AES3 frame format and AES5 sampling rates. Implementations exist as separate repositories.

## Decision

Reuse AES3-2009 and AES5-2018 repositories directly via CMake (FetchContent or submodules). Enforce namespace isolation and prohibit reimplementation in this repo.

## Consequences

### Positive

- Eliminates duplication and divergence
- Accelerates development with proven code

### Negative

- Dependency management requires version pinning and CI validation

## Alternatives Considered

- Reimplement AES3/AES5 locally – rejected (duplication, maintenance risk)

## Compliance and Standards

- Requirements: REQ-F-INTEG-001..004, REQ-NF-PORT-003

## Implementation Notes

- Pin versions and document compatibility matrix
- Provide adapter layer if minor API differences arise

## Review and Approval

- Proposed by: Architecture Team
- Approved by: Technical Lead
- Date: 2025-11-07
