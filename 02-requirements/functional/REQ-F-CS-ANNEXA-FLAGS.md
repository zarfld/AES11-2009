---
specType: requirements
standard: "29148"
phase: 02-requirements
version: 1.0.0
author: "AI-Assisted"
date: 2025-11-08
status: draft
traceability:
  stakeholderRequirements:
    - StR-CS-002
---

# REQ-F-CS-ANNEXA-FLAGS: UTC and Auxiliary Flag Mapping

## Statement

The implementation SHALL provide individual read/write flag abstractions for UTC-valid, leap-second-pending, DST-active, non-audio-frame, and alignment-marker indicators within a single designated channel status byte, preserving other bits unmodified.

## Rationale

Isolating each semantic flag reduces coupling, improves testability, and mitigates FM-CS-002 (bit overlap) and FM-CS-009 (drift in flag positions) by centralizing masks.

## Fit Criterion / Acceptance Tests

- When a flag is set then read via its accessor, the returned value SHALL equal the set value without altering unrelated bits in the same byte.
- Clearing a flag SHALL restore only that bit to 0 while all other bits retain their prior state.
- Simultaneous setting of multiple flags in sequence SHALL yield the expected combined bit pattern.

## Dependencies

- DES-C-001 Channel Status Mapping Utilities
- REQ-F-CS-ANNEXA-LEN (validates buffer length prerequisite)

## Verification Methods

- Unit tests: `ChannelStatusAnnexATests.DstFlagRoundTrip`, `ChannelStatusAnnexATests.NonAudioFlagRoundTrip`, `ChannelStatusAnnexATests.AlignmentMarkerFlagRoundTrip`, `ChannelStatusAnnexATests.PreservesReservedBytesOutsideMappedFields`.
- Mask uniqueness test: `ChannelStatusConstantsTests.MasksAreUniqueAndNonOverlapping`.

## Risk / Failure Modes Addressed

- FM-CS-002 Bit overlap collision
- FM-CS-009 Untracked flag migration

## Notes

Flag positions are implementation-defined and may be realigned if authoritative specification mapping differs; tests enforce preservation of untouched bits.
