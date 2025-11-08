---
specType: requirements
standard: "29148"
phase: 02-requirements
version: 1.0.0
author: "AI-Assisted"
date: "2025-11-08"
status: draft
traceability:
  stakeholderRequirements:
    - StR-FUNC-001
---

# REQ-F-CS-ANNEXA-LEN: Minimum Channel Status Block Length

Requirement ID: REQ-F-CS-ANNEXA-LEN
Aliases: REQ-F-CS-ANNEXA-001

## Statement

The implementation SHALL reject any channel status buffer shorter than 24 bytes for all Annex A mapping operations (UTC/timezone, flags, date/time fields) to prevent partial writes or invalid reads.

## Rationale

A fixed-length mapping ensures deterministic field offsets and mitigates failure modes FM-CS-001 (buffer underrun) and FM-CS-008 (partial field truncation). Enforcing the minimum upfront reduces branching complexity.

## Fit Criterion / Acceptance Tests

- Given a buffer length < 24, when `set_utc_info` is invoked, THEN the function returns false and no fields are modified.
- Given a buffer length < 24, when `set_datetime_info` is invoked, THEN the function returns false and date/time fields are not written.
- Given a buffer length < 24, when any flag setter (DST/non-audio/alignment) is invoked, THEN the function returns false.

## Dependencies

- DES-C-001 Channel Status Mapping Utilities

## Verification Methods

- Unit tests: `ChannelStatusAnnexATests.RejectsSubMinimumLengthBufferUtc`, `ChannelStatusAnnexATests.RejectsSubMinimumLengthBufferDateTime`.
- Code review for consistent length guard `length < kChannelStatusMinBytes`.

## Risk / Failure Modes Addressed

- FM-CS-001 Buffer underrun
- FM-CS-008 Partial field encoding

## Notes

Implementation defined mapping uses indices up to 19 for timezone; enforcing 24 bytes reserves space for future expansion and guards against later index drift.
