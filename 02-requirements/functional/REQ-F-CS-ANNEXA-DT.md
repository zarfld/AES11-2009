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
    - StR-CS-004
---

# REQ-F-CS-ANNEXA-DT: Date/Time Field Mapping

## Statement

The implementation SHALL encode six contiguous bytes representing (YY, MM, DD, HH, MM, SS) with validation of ranges and SHALL represent a leap second only by setting leapSecond=true with second==59.

## Rationale

Compact contiguous mapping reduces complexity and supports atomic validation. Leap second rule prevents invalid second=60 storage and mitigates FM-CS-004 (improper leap handling) and FM-CS-007 (range omission).

## Fit Criterion / Acceptance Tests

- Encoding a valid date/time (year 25, month 11, day 7, hour 14, minute 30, second 45) SHALL round-trip accurately.
- Attempting to encode month=13 or second=60 SHALL fail and not write fields.
- Attempting to encode leapSecond=true with second!=59 SHALL fail.

## Dependencies

- DES-C-001 Channel Status Mapping Utilities
- REQ-F-CS-ANNEXA-LEN

## Verification Methods

- Unit tests: `ChannelStatusDateTimeTests.EncodeDecodeUtcHappyPath`, `ChannelStatusDateTimeTests.RejectInvalidMonth`, `ChannelStatusDateTimeTests.RejectSecond60WithoutLeapFlag`, `ChannelStatusDateTimeTests.RejectLeapSecondFlagWhenSecondNot59`.

## Risk / Failure Modes Addressed

- FM-CS-004 Incorrect leap second representation
- FM-CS-007 Missing range validation

## Notes

Year stored as offset (0–99); external context determines century. Leap second indicated indirectly to avoid storing 60.
