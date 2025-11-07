---
id: DES-DARS-CAPTURE
title: DARS Capture Range Component
status: draft
version: 0.1.0
owner: standards-team
implements:
  - REQ-F-DARS-003
  - REQ-F-DARS-002
---

# DARS Capture Range Component

## Purpose

Implements capture range evaluation logic for Digital Audio Reference Signal (DARS) acquisition, determining whether the incoming reference signal frequency error falls within Grade 1 or Grade 2 acceptable capture bounds. Relates directly to frequency accuracy (Grade 1/2) and acquisition behavior.

## Context

* Acquisition state machine (see `dars_protocol.cpp`) needs a predicate to decide if measured frequency error permits transition from Acquire to Locked.
* Grade definitions: tighter bounds for professional (Grade 1) and wider bounds for broadcast (Grade 2). Implementation uses ppm thresholds only (no reproduction of spec text).

## Requirements Traceability

| Requirement | Relationship | Notes |
|-------------|-------------|-------|
| REQ-F-DARS-003 | Implements | Capture range ±2 ppm (Grade1), ±50 ppm (Grade2) for acquisition acceptance |
| REQ-F-DARS-002 | Supports | Frequency accuracy measurement feeds capture decision logic |

## Interfaces

### Public API

* `CaptureRange::within_capture(absPpmError, grade)` → bool
* `CaptureRange::ppm_error(expectedHz, measuredHz)` → double absolute ppm error

## Design Rationale

* Separation from state machine keeps evaluation pure and testable.
* Threshold constants inline with understanding of AES-11 grade capture ranges; can extend to hold ranges later.

## Algorithm

1. Compute absolute ppm error: `abs((measured - expected)/expected) * 1e6`.
2. Compare against grade-specific capture threshold.
3. Return boolean; caller decides state transitions.

## Data Structures

None beyond enum `Grade` and basic doubles.

## Error Handling

* Zero expectedHz returns `infinity` ppm (immediate rejection) to avoid divide-by-zero.

## Concurrency

* Pure stateless functions; thread-safe.

## Testing Strategy

| Test ID | Purpose | Requirement |
|---------|---------|-------------|
| TEST-DARS-CAPTURE-001 | Grade1 boundary below/above 2 ppm | REQ-F-DARS-003 / REQ-F-DARS-002 |
| TEST-DARS-CAPTURE-002 | Grade2 boundary below/above 50 ppm | REQ-F-DARS-003 |
| TEST-DARS-CAPTURE-003 | ppm error computation correctness | REQ-F-DARS-003 / REQ-F-DARS-002 |

## Future Extensions

* Add hold range evaluation method.
* Integrate jitter tolerance (links to REQ-NF-PERF-003).

## Traceability Notes

This design component forms a bridge between raw frequency measurement logic and acquisition state transitions, enabling authentic linkage of REQ-F-DARS-003 and REQ-F-DARS-002 to implementation and tests without inflated coverage.
