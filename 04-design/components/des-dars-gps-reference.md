---
id: DES-DARS-GPS
title: DARS GPS-Referenced Synchronization Component
status: draft
version: 0.1.0
owner: standards-team
implements:
  - REQ-F-DARS-006
supports:
  - REQ-F-HAL-004
---

## Purpose

Provide hardware-agnostic logic for aligning the DARS Timing Reference Point (TRP) to GPS 1PPS and evaluating alignment against a ±1 µs goal per AES-11 Section 4.2.4 understanding (no reproduction of spec text). Exposes predicates and phase-offset computation to be used by the synchronization manager.

## Context

- GPS 1PPS gives an absolute second boundary; DARS TRP can be aligned to that boundary.
- The component does not talk to hardware; a higher layer provides timestamps for TRP and 1PPS via abstract interfaces.
- Holdover / frequency disciplining lives in other components; this module only evaluates phase alignment.

## Interfaces

- `GPSReferenceSync::phase_offset_us(trpTimeNs, ppsTimeNs)` → double µs
- `GPSReferenceSync::within_alignment(trpTimeNs, ppsTimeNs, tolerance_us=1.0)` → bool

## Requirements Traceability

| Requirement | Relationship | Notes |
|-------------|-------------|-------|
| REQ-F-DARS-006 | Implements | 1PPS alignment evaluation ±1 µs |
| REQ-F-HAL-004 | Supports | Expects abstract 1PPS timestamps from HAL layer |

## Design Rationale

- Keep purely computational; stateless and testable.
- Tolerance configurable with default of 1 µs to support different operational profiles.

## Error Handling

- Caller responsible for providing valid nanosecond timestamps; function computes absolute differences.

## Testing Strategy

| Test ID | Purpose | Requirement |
|---------|---------|-------------|
| TEST-GPS-REF-001 | Check 0.2 µs offset is within ±1 µs | REQ-F-DARS-006 |
| TEST-GPS-REF-002 | Check 1.5 µs exceeds default tolerance | REQ-F-DARS-006 |
| TEST-GPS-REF-003 | Custom tolerance 2.0 µs accepts 1.5 µs | REQ-F-DARS-006 |

## Integration Notes

- Synchronization Manager uses `within_alignment` as a phase-alignment predicate when operating in SYNC_REF_GPS mode.
- Combine with CaptureRange/PhaseTolerance for full acquisition and steady-state maintenance.
