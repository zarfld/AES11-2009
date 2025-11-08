# Software Failure Modes Effects Analysis (SFMEA)

**Project**: AES11-2009 DARS Standards Layer  
**Component**: ChannelStatus Utilities (ChannelStatusUtils & related flag/date/time mapping)  
**Version**: 0.1.0  
**Date**: 2025-11-08  
**Document ID**: SFMEA-CHANNELSTATUS-0.1.0  
**Status**: Draft  
**IEEE 1633-2016 Compliant** (Clause 5.2, Annex A)

---

## Document Control

| Version | Date | Author | Changes | Approved By |
|---------|------|--------|---------|-------------|
| 0.1.0 | 2025-11-08 | AI Assistant | Initial draft SFMEA for Channel Status mapping | TBD |

## Table of Contents

1. Introduction  
2. SFMEA Scope and Approach  
3. Component/Function Description  
4. Failure Modes Identification  
5. Root Cause Analysis  
6. Consequences Analysis  
7. Risk Assessment (RPN)  
8. Mitigation Actions  
9. Critical Items List (CIL)  
10. SFMEA Validation  

---

## 1. Introduction

### 1.1 Purpose

This SFMEA analyzes potential software failure modes for channel status parsing, encoding, and flag handling in the AES11-2009 DARS implementation to:

- Identify failure modes early
- Assess risk (Severity × Likelihood × Detectability)
- Define mitigation actions before integration scaling
- Produce a Critical Items List (CIL) for tracking

### 1.2 SFMEA Benefits (IEEE 1633 Clause 5.2.1)

- Proactive defect prevention
- Higher reliability of synchronization metadata
- Risk-driven test expansion
- Reduced downstream integration rework

### 1.3 Relationship to Other Documents

- **Source Documents**:  
  - System Requirements: `02-requirements/system-requirements-specification.md`  
  - DARS Functional Requirements (subset): REQ-F-DARS-007, REQ-F-DARS-008 (timing, classification)  
  - Design Components: (future delta doc to add mapped flag layout)  
- **Output Documents**:  
  - Critical Items List (Section 9)  
  - Added/Updated Tests (unit & robustness)  
  - Design delta (planned) in `04-design/components/`  

---

## 2. SFMEA Scope and Approach

### 2.1 Scope

**Component Analyzed**: ChannelStatusUtils & associated mapping constants (not yet centralized)  
**Version**: Current development tip  
**Analysis Level**: Code + Low-level design  

**Included**:

- Grade extraction/set
- UTC & leap second flags
- Timezone offset encode/decode
- Date/time encode/decode (year, month, day, hour, minute, second, leap semantics)
- DST / Non-Audio / Alignment marker flags (implementation-defined bits)

**Excluded** (deferred / separate SFMEA):

- Higher-level frame assembly
- External AES3 / AES5 submodule correctness (tested elsewhere)
- Video sync alignment logic

### 2.2 SFMEA Process (IEEE 1633 Clause 5.2.2)

```text
Failure Mode Identification → Root Causes → Consequences → Risk (RPN) → Mitigations → CIL
```

### 2.3 SFMEA Team (Proposed Roles)

| Role | Name | Contribution |
|------|------|--------------|
| SFMEA Lead | TBD | Coordination |
| Requirements Engineer | TBD | Req traceability |
| Designer/Architect | TBD | Mapping decisions |
| Developer | TBD | Code insight |
| Tester | TBD | Detectability scoring |
| Reliability Engineer | TBD | RPN sanity |

 

### 2.4 Sessions (Planned)

| Session # | Date | Duration | Scope | Participants |
|-----------|------|----------|-------|--------------|
| 1 | 2025-11-10 | 1.5h | Flag & TZ mapping | TBD |
| 2 | 2025-11-12 | 1.0h | Mitigation review | TBD |

---
## 3. Component/Function Description

### 3.1 Component Overview
**Component ID**: SW-CS-UTIL  
**Name**: ChannelStatus Utilities  
**Description**: Provides helper functions to extract and write AES3/AES11-related metadata fields (grade, UTC validity, leap second, DST, non-audio, alignment marker, timezone minutes, date/time tuple) into a 24-byte channel status buffer.

### 3.2 Interfaces (Logical)

| Interface ID | Name | Type | Connected To | Protocol |
|--------------|------|------|--------------|----------|
| IF-CS-RAW | RawChannelStatusBuffer | Data | DARS frame assembly | In-memory struct |
| IF-CS-TEST | Unit Test Harness | Test | GTest | Test API |

### 3.3 Data Elements

| Data Element | Type | Range/Format | Source | Destination | Criticality |
|--------------|------|--------------|--------|-------------|-------------|
| grade bits | uint2 | {00-11} | Protocol logic | Channel status | High |
| utcValid | bool | {0,1} | External time source | Channel status | High |
| leapSecond | bool | {0,1} | Timing service | Channel status | High |
| dstFlag | bool | {0,1} | Regional config | Channel status | Medium |
| nonAudioFlag | bool | {0,1} | Frame classification | Channel status | Medium |
| alignmentMarker | bool | {0,1} | Sync pipeline | Channel status | High |
| timezoneOffsetMinutes | int16 | [-720..840] | Config/time service | Channel status | High |
| dateTime fields | struct | BCD-like raw bytes | Time service | Channel status | High |

### 3.4 Operating Variables

| Variable | Description | Valid Range | Initial Value | Update Frequency |
|----------|-------------|-------------|---------------|------------------|
| bufferLength | Provided buffer size | ≥24 | Caller-defined | Per frame/update |
| sampleRate | External dependency reference | Standard rates | Varies | On sync events |

---
 

## 4. Failure Modes Identification

### 4.1 Categories (IEEE 1633 Annex A)

(Faulty Data, Faulty Timing, Faulty Sequencing, Faulty Error Handling, Faulty Logic)

### 4.2 Failure Modes Table

| FM ID | Failure Mode | Category | Description | Requirement Violated (Placeholder) |
|-------|--------------|----------|-------------|------------------------------------|
| FM-CS-001 | Buffer underrun length check missed | Faulty Error Handling | set_* writes past provided length <24 | REQ-F-CS-ANNEXA-LEN |
| FM-CS-002 | Incorrect mask overlap (DST vs Non-Audio) | Faulty Logic | Bit definitions collide causing false reads | REQ-F-CS-ANNEXA-FLAGS |
| FM-CS-003 | Timezone overflow sign misinterpret | Faulty Data | Negative offset encoded/decoded incorrectly | REQ-F-CS-ANNEXA-TZ |
| FM-CS-004 | Leap second flag set with second !=59 accepted | Faulty Logic | Invalid semantic accepted unflagged | REQ-F-CS-ANNEXA-DT |
| FM-CS-005 | Grade misclassification reserved bits | Faulty Logic | Reserved pattern not handled → wrong grade | REQ-F-DARS-008 |
| FM-CS-006 | Partial update leaves mixed old/new flags | Faulty Sequencing | Multiple flags written without atomic semantics | REQ-F-CS-ANNEXA-ATOMIC |
| FM-CS-007 | Unvalidated datetime ranges (month=0) | Faulty Data | Invalid month/day accepted | REQ-F-CS-ANNEXA-DT |
| FM-CS-008 | Date/time extraction silently returns values on invalid buffer size | Faulty Error Handling | Caller assumes success with truncated buffer | REQ-F-CS-ANNEXA-LEN |
| FM-CS-009 | Alignment marker mis-set due to stale mask constant | Faulty Logic | Constant drift after refactor | REQ-F-CS-ANNEXA-FLAGS |
| FM-CS-010 | Inconsistent timezone/leap combination not flagged | Faulty Error Handling | Contradictory temporal state passes silently | REQ-F-CS-ANNEXA-TZ |

---
 

## 5. Root Cause Analysis

| FM ID | Potential Root Causes | Root Cause Category |
|-------|-----------------------|---------------------|
| FM-CS-001 | Missing length guard; future refactor removing check; absent negative test | Implementation, Verification |
| FM-CS-002 | Masks defined inline in multiple places; no central header | Design, Implementation |
| FM-CS-003 | Cast/endianness misuse; forgetting sign extension | Implementation |
| FM-CS-004 | Incomplete semantic validation of leapSecond precondition | Requirements, Implementation |
| FM-CS-005 | Switch omission; no default test for reserved pattern | Implementation, Verification |
| FM-CS-006 | Separate sequential writes without staging buffer | Design |
| FM-CS-007 | No comprehensive range validation; boundary test gaps | Verification, Implementation |
| FM-CS-008 | Insufficient early return; ambiguous return convention | Design, Implementation |
| FM-CS-009 | Hardcoded literal instead of shared constant | Implementation |
| FM-CS-010 | Lack of cross-field consistency rule function | Design |

---
 

## 6. Consequences Analysis

| FM ID | Local Effect | System Effect | End User Effect | Severity (S) |
|-------|--------------|---------------|-----------------|--------------|
| FM-CS-001 | Memory corruption risk | Crash/undefined behavior | Service interruption | 9 |
| FM-CS-002 | Flags misreported | Wrong sync mode downstream | Drift / misalignment | 8 |
| FM-CS-003 | Wrong TZ offset | Time shift in logs/events | Confusing timestamps | 7 |
| FM-CS-004 | Invalid leap handling | Timing alignment edge errors | Potential sync glitch | 8 |
| FM-CS-005 | Incorrect grade classification | Misapplied tolerances | Stability degradation | 8 |
| FM-CS-006 | Inconsistent visible state | Transient logic errors | Intermittent faults | 7 |
| FM-CS-007 | Accept invalid date/time | Scheduling anomaly | Incorrect time metadata | 6 |
| FM-CS-008 | Silent failure propagation | Hidden invalid state | Latent failure | 7 |
| FM-CS-009 | Alignment marker unreliable | Mis-timed reference alignment | Phase jitter accumulation | 8 |
| FM-CS-010 | Temporal inconsistency | Downstream rejection or drift | User-facing instability | 7 |

---
 

## 7. Risk Assessment (RPN)

Rating Scales: Severity (S) per table above; Likelihood (L) initial guess (1–10); Detectability (D): higher = harder to detect in current tests.

| FM ID | S | L | D | RPN (S×L×D) | Risk Level |
|-------|---|---|---|-------------|-----------|
| FM-CS-001 | 9 | 4 | 7 | 252 | CRITICAL |
| FM-CS-002 | 8 | 5 | 6 | 240 | CRITICAL |
| FM-CS-003 | 7 | 4 | 6 | 168 | HIGH |
| FM-CS-004 | 8 | 5 | 5 | 200 | CRITICAL |
| FM-CS-005 | 8 | 4 | 5 | 160 | HIGH |
| FM-CS-006 | 7 | 5 | 6 | 210 | CRITICAL |
| FM-CS-007 | 6 | 4 | 4 | 96 | MEDIUM |
| FM-CS-008 | 7 | 4 | 6 | 168 | HIGH |
| FM-CS-009 | 8 | 3 | 6 | 144 | HIGH |
| FM-CS-010 | 7 | 4 | 6 | 168 | HIGH |

RPN Thresholds: ≥200 Critical; 100–199 High; 50–99 Medium; <50 Low.

---
 

## 8. Mitigation Actions

| FM ID | RPN Before | Mitigation Actions (Ordered) | Strategy | Owner | Target | Residual S | Residual L | Residual D | Residual RPN | Risk Reduction % |
|-------|------------|------------------------------|----------|-------|--------|-----------|------------|------------|--------------|-----------------|
| FM-CS-001 | 252 | Centralized bounds helper + static_assert + add explicit negative length test | Prevention | TBD | 2025-11-15 | 9 | 2 | 3 | 54 | 78.6% |
| FM-CS-002 | 240 | Introduce single mapping header & refactor; add mask uniqueness test | Prevention/Detection | TBD | 2025-11-15 | 8 | 2 | 2 | 32 | 86.7% |
| FM-CS-003 | 168 | Add signed decode helper + fuzz tests for edge offsets | Prevention/Detection | TBD | 2025-11-16 | 7 | 2 | 3 | 42 | 75.0% |
| FM-CS-004 | 200 | Enforce leap precondition (second==59) + test invalid combos | Prevention | TBD | 2025-11-16 | 8 | 2 | 3 | 48 | 76.0% |
| FM-CS-005 | 160 | Add explicit reserved grade branch + test | Detection | TBD | 2025-11-14 | 8 | 2 | 2 | 32 | 80.0% |
| FM-CS-006 | 210 | Implement staged write pattern (local struct then commit) | Prevention | TBD | 2025-11-20 | 7 | 2 | 4 | 56 | 73.3% |
| FM-CS-007 | 96 | Expand range validation (day-month combos) + boundary tests | Prevention | TBD | 2025-11-18 | 6 | 2 | 3 | 36 | 62.5% |
| FM-CS-008 | 168 | Standardize return semantics (bool/optional) doc + failing tests for truncated buffer | Detection | TBD | 2025-11-14 | 7 | 2 | 3 | 42 | 75.0% |
| FM-CS-009 | 144 | Replace literals with constants + unit test verifying constants set | Prevention/Detection | TBD | 2025-11-14 | 8 | 2 | 2 | 32 | 77.8% |
| FM-CS-010 | 168 | Add cross-field consistency validator (timezone/leap coherence) + tests | Detection/Prevention | TBD | 2025-11-19 | 7 | 2 | 3 | 42 | 75.0% |

---
 

## 9. Critical Items List (CIL)

(Critical = RPN ≥ 200 before mitigation)

| CIL ID | FM ID | Failure Mode | RPN | Mitigation Summary | Status | Verification Artifact |
|--------|-------|--------------|-----|--------------------|--------|-----------------------|
| CIL-CS-001 | FM-CS-001 | Buffer underrun length check missed | 252 | Bounds helper + static_assert + negative test | Open | New test: `ChannelStatusAnnexATests.InvalidBufferLength` |
| CIL-CS-002 | FM-CS-002 | Incorrect mask overlap | 240 | Central mapping header + uniqueness test | Open | New test: `ChannelStatusAnnexATests.MaskUniqueness` |
| CIL-CS-003 | FM-CS-004 | Leap second flag misuse | 200 | Precondition enforcement test | Open | New test: `ChannelStatusDateTimeTests.LeapSecondPrecondition` |
| CIL-CS-004 | FM-CS-006 | Partial update inconsistent state | 210 | Staged write commit pattern | Open | New test: `ChannelStatusAtomicWriteTests.MultiFlagConsistency` |

---
 

## 10. SFMEA Validation

### 10.1 Checklist

- [x] Failure modes enumerated across five categories
- [x] Initial RPN assigned
- [x] Critical items extracted
- [ ] Traceability IDs finalized (placeholder REQ-F-CS-ANNEXA-*)
- [ ] Owners assigned
- [ ] Mitigation tests implemented

 

### 10.2 Coverage Metrics (Initial)

| Metric | Count | Target | Status |
|--------|-------|--------|--------|
| Failure Modes Identified | 10 | N/A | OK |
| Critical Items (≥200) | 4 | N/A | OK |
| Mitigation Actions Defined | 10 | = FM count | OK |
| Traceability Linked | 0 (placeholders) | 100% | Pending |

 

### 10.3 Independent Review

| Reviewer | Role | Date | Findings | Status |
|----------|------|------|----------|--------|
| TBD | QA | - | - | Pending |
| TBD | Reliability Eng | - | - | Pending |

 

### 10.4 Lessons Learned (Preliminary)

**What Works**: Early SFMEA exposes need for central mapping constants.  
**Improve**: Formal requirement IDs for channel status mapping semantics.  
**Next Actions**: Implement constants header + negative & cross-field tests.

---
 

## Appendix A: Reference to IEEE 1633 Annex A Categories

Applied categories: Faulty Data, Faulty Timing (none high severity yet), Faulty Sequencing, Faulty Error Handling, Faulty Logic.

## Appendix B: Root Cause Keywords Applied

Implementation, Design, Verification, Requirements.

---
---

END OF SFMEA
