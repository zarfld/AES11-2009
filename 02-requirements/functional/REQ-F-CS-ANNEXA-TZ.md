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

# REQ-F-CS-ANNEXA-TZ: Timezone Offset Encoding

Requirement ID: REQ-F-CS-ANNEXA-TZ
Aliases: REQ-F-CS-ANNEXA-004

## Statement

The implementation SHALL encode a signed timezone offset in minutes using two consecutive little-endian bytes and SHALL reject offsets outside the inclusive range [-720, 840].

## Rationale

Restricting to globally valid UTC offsets prevents nonsensical values and mitigates FM-CS-003 (sign-extension errors) by enforcing a bounded, validated domain.

## Fit Criterion / Acceptance Tests

- Setting UTC info with timezoneOffsetMinutes = 120 SHALL persist +120 and decode to +120.
- Setting UTC info with timezoneOffsetMinutes = -300 SHALL persist -300 and decode to -300.
- An attempt to set timezoneOffsetMinutes = 900 SHALL return false and not modify stored bytes.

## Dependencies

- DES-C-001 Channel Status Mapping Utilities
- REQ-F-CS-ANNEXA-LEN

## Verification Methods

- Unit tests: `ChannelStatusAnnexATests.UtcInfoRoundTripWithinBounds`, `ChannelStatusAnnexATests.UtcInfoNegativeOffsetRoundTrip`, `ChannelStatusAnnexATests.RejectsOutOfRangeTimezone`.
- Future fuzz tests for min/max boundary values.

## Risk / Failure Modes Addressed

- FM-CS-003 Sign or range corruption of timezone offset

## Notes

Range limits chosen to cover UTC-12 through UTC+14 timezones; representation uses two's complement of int16.
