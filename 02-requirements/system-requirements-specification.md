---
specType: requirements
standard: 29148
phase: 02-requirements
version: 1.0.0
author: AI Assistant
date: 2025-11-07
status: draft
traceability:
  stakeholderRequirements:
    - StR-BUS-001
    - StR-BUS-002
    - StR-BUS-003
    - StR-BUS-004
    - StR-USER-001
    - StR-USER-002
    - StR-USER-003
    - StR-USER-004
    - StR-FUNC-001
    - StR-FUNC-002
    - StR-FUNC-003
    - StR-FUNC-004
    - StR-PERF-001
    - StR-PERF-002
    - StR-PERF-003
    - StR-PERF-004
    - StR-PERF-005
    - StR-COMP-001
    - StR-COMP-002
    - StR-COMP-003
    - StR-COMP-004
    - StR-COMP-005
    - StR-REL-001
    - StR-REL-002
    - StR-REL-003
    - StR-REL-004
    - StR-REL-005
    - StR-REL-006
---

# System Requirements Specification (SyRS)

**Project**: AES-11-2009 Hardware-Agnostic Implementation  
**Document ID**: SyRS-AES11-001  
**Version**: 1.0  
**Date**: 2025-11-07  
**Status**: Draft

**Standard Compliance**: ISO/IEC/IEEE 29148:2018 (System Requirements), ISO/IEC/IEEE 12207:2017

---

## 1. Introduction

### 1.1 Purpose

This System Requirements Specification (SyRS) transforms the stakeholder requirements documented in the Stakeholder Requirements Specification (StRS-AES11-001) into detailed, testable system requirements. These requirements define WHAT the AES-11-2009 DARS implementation system shall do, specifying functional behavior, performance characteristics, interfaces, constraints, and quality attributes necessary for successful implementation.

This document serves as:
- **Contract** between stakeholders and development team
- **Baseline** for architecture and design decisions
- **Foundation** for test case development
- **Reference** for verification and validation activities

### 1.2 Scope

**In Scope**:
- Detailed functional requirements for DARS protocol implementation
- Hardware Abstraction Layer (HAL) interface specifications
- Performance requirements with measurable metrics
- Reliability requirements with quantitative targets
- Security and compliance requirements
- Integration requirements for external standards (AES3-2009, AES5-2018)
- Conformance testing requirements
- Developer experience requirements

**Out of Scope**:
- Implementation design decisions (HOW - covered in Phase 03 Architecture)
- Platform-specific optimizations (isolated in service layer)
- User interface designs (no GUI required)
- AES3 digital audio interface implementation (external dependency)
- AES5 sampling frequency definitions (external dependency)
- Complete AES67 Audio-over-IP features (future enhancement)

### 1.3 Definitions, Acronyms, and Abbreviations

| Term | Definition |
|------|------------|
| **System Requirement** | Detailed specification of system capability, performance, or constraint derived from stakeholder requirements |
| **REQ-F-xxx** | Functional requirement identifier format |
| **REQ-NF-xxx** | Non-functional requirement identifier format |
| **Shall** | Mandatory binding provision (normative) |
| **Should** | Recommended provision (advisory) |
| **May** | Optional provision (permissible) |
| **DARS** | Digital Audio Reference Signal per AES-11-2009 |
| **HAL** | Hardware Abstraction Layer - platform-independent interface |
| **TRP** | Timing Reference Point - X/Z preamble transition in AES3 frame (AES-11 Section 3.3) |
| **Grade 1** | High-accuracy DARS with ±1 ppm frequency tolerance (AES-11 Section 5.1.2.1) |
| **Grade 2** | Standard DARS with ±10 ppm frequency tolerance (AES-11 Section 5.1.2.2) |
| **Synchronism** | Frame frequency and phase identical for two digital audio signals (AES-11 Section 3.1) |
| **Capture Range** | Minimum oscillator lock range (Grade 1: ±2 ppm, Grade 2: ±50 ppm per AES-11 Section 5.2.2) |

### 1.4 References

**Normative References** (Requirements Source):
- **StRS-AES11-001**: Stakeholder Requirements Specification v1.0 (2025-11-07)
- **AES-11-2009 (R2014)**: AES recommended practice for digital audio engineering - Synchronization of digital audio equipment in studio operations
- **AES3-2009**: AES standard for digital audio - Serial transmission format for linearly represented digital audio data
- **AES5-2018**: AES recommended practice - Preferred sampling frequencies for applications employing pulse-code modulation
- **IEEE 1633-2016**: IEEE Recommended Practice on Software Reliability
- **ISO/IEC/IEEE 29148:2018**: Systems and software engineering - Life cycle processes - Requirements engineering
- **ISO/IEC/IEEE 12207:2017**: Systems and software engineering - Software life cycle processes

**Informative References**:
- **IEEE 1588-2019**: IEEE Standard for a Precision Clock Synchronization Protocol (PTP) - for GPS-referenced synchronization
- **SMPTE 318M-1999**: Synchronization of 59.94 or 50 Hz related video and audio systems
- **Writing Effective Use Cases** - Alistair Cockburn

### 1.5 Overview

This document is organized following ISO/IEC/IEEE 29148:2018 structure:

- **Section 2**: System Overview - Context, capabilities, interfaces, constraints
- **Section 3**: Functional Requirements (REQ-F-xxx) - Organized by feature area
- **Section 4**: Non-Functional Requirements (REQ-NF-xxx) - Performance, reliability, security, etc.
- **Section 5**: System Interfaces - HAL specifications, external integrations
- **Section 6**: Constraints - Technical, standards compliance, project limitations
- **Section 7**: Requirements Traceability Matrix - StR → REQ mapping
- **Section 8**: Verification Approach - How requirements will be validated

---

## 2. System Overview

### 2.1 System Context

The AES-11-2009 DARS implementation system operates within a professional audio studio environment, providing synchronization services to digital audio equipment from multiple manufacturers. The system:

- **Receives**: DARS signals via AES3 interface, video reference signals, GPS timing pulses, or digital audio inputs
- **Processes**: Protocol state machines, timing calculations, phase relationships, conformance validation
- **Outputs**: Synchronized sample clocks, DARS signals, diagnostic data, conformance reports
- **Interfaces**: Hardware via HAL abstraction, external standards via Git repositories, developer tools via APIs

**System Boundaries**:
- **Inside**: AES-11 protocol logic, DARS state machines, timing calculations, HAL interfaces, conformance tests
- **Outside**: Hardware drivers (audio interfaces, timers, clocks), AES3 frame processing, AES5 sampling rates, platform OS services

### 2.2 System Capabilities

**Primary Capabilities**:
1. **DARS Protocol Implementation**: Support all 4 synchronization methods per AES-11 Section 4.2
2. **Grade 1/2 Timing Precision**: Frequency accuracy ±1 ppm (Grade 1) or ±10 ppm (Grade 2) per Section 5.2
3. **Hardware Abstraction**: Platform-independent protocol logic with HAL for hardware access
4. **Conformance Testing**: Automated validation of AES-11-2009 normative requirements
5. **External Standards Integration**: Seamless integration with AES3-2009 and AES5-2018 implementations

**Secondary Capabilities**:
1. **Real-time Diagnostics**: Monitoring APIs for lock status, timing accuracy, jitter measurements
2. **Developer Tools**: HAL validator, timing analyzer, conformance checker utilities
3. **Comprehensive Documentation**: API reference, integration tutorials, conformance guides

### 2.3 User Classes and Characteristics

**Primary Users** (from StRS):
- **Audio Software Developers**: Integrate AES-11 into applications - need easy API, fast integration (<4 hours)
- **Audio Equipment Manufacturers**: Embed in products - need certification-ready, production-quality reliability
- **System Integrators**: Build multi-vendor systems - need diagnostic tools, interoperability guarantees

**Operational Characteristics**:
- **Skill Level**: Professional audio engineers familiar with digital audio standards
- **Usage Frequency**: Continuous operation in live production and studio recording environments
- **Environment**: Professional studio (Grade 1) or live sound (Grade 2) with real-time constraints

### 2.4 Operating Environment

**Platform Support**:
- **Embedded**: ARM Cortex-M7 @216 MHz (bare metal RTOS)
- **Desktop**: x86-64 (Linux, Windows)
- **Real-time Constraints**: <10 µs latency in audio callback paths

**Resource Constraints**:
- **Memory**: <64 KB static allocation for embedded targets
- **Processing**: Zero dynamic memory allocation during operation
- **Timing**: Microsecond-precision timer access via HAL

### 2.5 Assumptions and Dependencies

**Assumptions**:
- AES-11-2009 specification remains stable (reaffirmed 2014, no major revisions expected)
- Target platforms provide microsecond-resolution timers
- Developers have basic C/C++ proficiency and audio standards familiarity
- Professional audio equipment manufacturers require AES-11 certification

**Dependencies** (from StRS Section 7.2):
- **D1**: AES3-2009 repository (https://github.com/zarfld/AES3-2009.git) - Ready
- **D2**: AES5-2018 repository (https://github.com/zarfld/AES5-2018.git) - Ready
- **D3**: IEEE 1588-2019 PTP (https://github.com/zarfld/IEEE_1588_2019.git) - In Development (optional)
- **D4**: CMake 3.14+ for build system
- **D5**: GoogleTest (C++) and Unity (C) for testing frameworks

---

## 3. Functional Requirements

Functional requirements define WHAT the system shall do, organized by feature area. All requirements follow the format:

**REQ-F-[AREA]-[NNN]: [Requirement Title]**

- **Trace to**: StR-XXX (stakeholder requirement)
- **Description**: Clear statement of capability
- **Priority**: Critical (P0) / High (P1) / Medium (P2) / Low (P3)
- **Rationale**: Why this requirement exists
- **Acceptance Criteria**: Given-When-Then scenarios
- **Dependencies**: Other requirements
- **AES-11 Reference**: Specific section/table/annex

### 3.1 DARS Protocol Requirements

Requirements for core Digital Audio Reference Signal protocol implementation per AES-11-2009.

#### REQ-F-DARS-001: DARS Format Compliance

- **Trace to**: StR-FUNC-001, StR-COMP-001
- **Priority**: Critical (P0)
- **AES-11 Reference**: Section 5.1.1

**Description**: The system shall generate and receive DARS signals conforming to AES3 two-channel digital audio interface format with valid preamble structure.

**Rationale**: AES-11-2009 Section 5.1.1 mandates DARS use AES3 format or minimal preamble-only structure for maximum compatibility.

**Functional Behavior**:
1. System shall accept AES3-compliant DARS signals via HAL audio interface
2. System shall parse X/Y/Z preambles per AES3 specification
3. System shall extract Timing Reference Point (TRP) from X or Z preamble initial transition
4. System shall identify channel status byte 4 (bits 0-1) for DARS grade identification
5. System shall generate DARS output signals with valid AES3 frame structure
6. System shall support preamble-only DARS format (minimal implementation)
7. System shall validate frame alignment within specified tolerances

**Boundary Values**:
- **Input DARS Format**: Full AES3 frame (192 bits) OR preamble-only (8 bits + padding)
- **Preamble Detection Threshold**: Valid X/Y/Z pattern per AES3-3-2009 Section 7
- **Frame Period**: 32 kHz = 31.25 µs, 44.1 kHz = 22.68 µs, 48 kHz = 20.83 µs, 96 kHz = 10.41 µs (per AES-11 Table 2)
- **Channel Status Byte 4 bits 0-1**: 00=default, 01=Grade 1, 10=Grade 2, 11=reserved

**Error Handling**:
| Error Condition | System Action | User Notification | Log Level |
|----------------|---------------|-------------------|-----------|
| Invalid preamble pattern | Reject frame, maintain previous lock | `DARS_ERROR_INVALID_PREAMBLE` | ERROR |
| Missing channel status | Use default grade (00) | `DARS_WARNING_NO_CHANNEL_STATUS` | WARNING |
| Unsupported DARS grade (11) | Reject DARS, report error | `DARS_ERROR_RESERVED_GRADE` | ERROR |
| Frame alignment violation | Trigger realignment, count violations | `DARS_WARNING_FRAME_MISALIGNMENT` | WARNING |

**Acceptance Criteria**:
```gherkin
Scenario: Parse valid Grade 1 DARS signal
  Given a DARS signal with Grade 1 identification (channel status byte 4 = 01)
  And valid X preamble structure per AES3
  When system receives DARS frame via HAL audio interface
  Then system shall extract TRP from X preamble initial transition
  And system shall identify DARS as Grade 1
  And system shall accept DARS for synchronization

Scenario: Generate DARS output with valid AES3 format
  Given system is synchronized to reference clock
  And target sampling frequency is 48 kHz
  When system generates DARS output signal
  Then DARS shall have valid X/Y/Z preambles per AES3-3-2009
  And frame period shall be 20.83 µs ± 1 ns
  And channel status byte 4 shall indicate correct grade (01 or 10)

Scenario: Handle preamble-only DARS format
  Given a minimal DARS signal with preambles only (no audio data)
  When system receives preamble-only DARS
  Then system shall extract TRP correctly
  And system shall lock to DARS frequency
  And system shall not require full AES3 frame structure

Scenario: Reject invalid preamble pattern
  Given a DARS signal with corrupted preamble (invalid bit pattern)
  When system attempts preamble detection
  Then system shall reject frame
  And system shall log ERROR with code `DARS_ERROR_INVALID_PREAMBLE`
  And system shall maintain previous synchronization lock state
  And system shall increment invalid_preamble_count diagnostic counter
```

**Dependencies**:
- **External**: AES3-2009 repository for preamble detection and frame format parsing
- **Internal**: REQ-F-HAL-001 (audio interface), REQ-NF-PERF-001 (timing precision)

**Verification Method**: Unit tests with mock AES3 frames, conformance test suite validation, interoperability testing with reference DARS sources

---

#### REQ-F-DARS-002: Grade 1/2 Frequency Accuracy

- **Trace to**: StR-PERF-001, StR-FUNC-001
- **Priority**: Critical (P0)
- **AES-11 Reference**: Section 5.2.1

**Description**: The system shall maintain long-term frequency accuracy within ±1 ppm (Grade 1) or ±10 ppm (Grade 2) relative to rated sampling frequency when generating DARS signals.

**Rationale**: AES-11-2009 Section 5.2.1 defines two performance grades enabling different studio requirements (multi-studio complex vs single studio).

**Functional Behavior**:
1. System shall provide configuration option to select Grade 1 or Grade 2 operation mode
2. When configured as Grade 1, system shall maintain frequency accuracy ±1 ppm over 24-hour stability period
3. When configured as Grade 2, system shall maintain frequency accuracy ±10 ppm over 24-hour stability period
4. System shall lock to external reference meeting grade specifications
5. System shall measure and report actual frequency accuracy via diagnostic API
6. System shall identify generated DARS grade in channel status byte 4 (bits 0-1)
7. System shall reject external DARS exceeding grade specifications during lock acquisition

**Boundary Values**:
- **Grade 1 Frequency Tolerance**: ±1 ppm of nominal sampling frequency
  - At 48 kHz: ±0.048 Hz (47.999952 Hz to 48.000048 Hz)
  - At 44.1 kHz: ±0.0441 Hz (44.0999559 Hz to 44.1000441 Hz)
- **Grade 2 Frequency Tolerance**: ±10 ppm of nominal sampling frequency
  - At 48 kHz: ±0.48 Hz (47.99952 Hz to 48.00048 Hz)
  - At 44.1 kHz: ±0.441 Hz (44.09956 Hz to 44.10044 Hz)
- **Measurement Period**: Minimum 24-hour continuous operation for long-term stability verification
- **Temperature Range**: Per manufacturer operating specifications (typically 0°C to 50°C)

**Error Handling**:
| Error Condition | System Action | User Notification | Log Level |
|----------------|---------------|-------------------|-----------|
| Frequency drift exceeds grade | Enter warning state, continue operation | `DARS_WARNING_FREQUENCY_DRIFT` | WARNING |
| External DARS exceeds grade spec | Reject DARS, maintain free-running | `DARS_ERROR_FREQUENCY_OUT_OF_RANGE` | ERROR |
| Grade 1 equipment receives Grade 2 DARS | Accept if within ±1 ppm, warn if outside | `DARS_WARNING_GRADE_MISMATCH` | WARNING |
| Temperature exceeds operating range | Report environmental error | `DARS_ERROR_ENVIRONMENTAL_LIMIT` | CRITICAL |

**Acceptance Criteria**:
```gherkin
Scenario: Grade 1 frequency accuracy over 24 hours
  Given system configured as Grade 1 DARS generator
  And nominal sampling frequency 48 kHz
  And stable temperature environment (25°C ± 2°C)
  When system operates continuously for 24 hours
  Then measured frequency shall remain within 48 kHz ± 0.048 Hz (±1 ppm)
  And frequency stability shall be verifiable via diagnostic API
  And channel status byte 4 bits 0-1 shall indicate Grade 1 (01)

Scenario: Grade 2 frequency accuracy over 24 hours
  Given system configured as Grade 2 DARS generator
  And nominal sampling frequency 48 kHz
  When system operates continuously for 24 hours
  Then measured frequency shall remain within 48 kHz ± 0.48 Hz (±10 ppm)
  And system shall meet Grade 2 specifications
  And channel status byte 4 bits 0-1 shall indicate Grade 2 (10)

Scenario: Grade 1 equipment rejects out-of-spec external DARS
  Given Grade 1 equipment
  And external DARS frequency 48.001 kHz (+20.83 ppm, exceeds ±1 ppm)
  When system attempts to lock to external DARS
  Then system shall reject DARS
  And system shall log ERROR with code `DARS_ERROR_FREQUENCY_OUT_OF_RANGE`
  And system shall report "External DARS exceeds Grade 1 tolerance"
  And system shall remain in free-running mode

Scenario: Grade mismatch warning
  Given Grade 1 equipment with ±1 ppm requirement
  And external DARS identified as Grade 2 (channel status 10)
  And actual external DARS frequency within ±1 ppm
  When system locks to external DARS
  Then system shall accept DARS (within Grade 1 tolerance)
  And system shall log WARNING with code `DARS_WARNING_GRADE_MISMATCH`
  And system shall report "Grade 2 DARS accepted by Grade 1 equipment"
```

**Dependencies**:
- **External**: High-precision reference clock (atomic clock, GPS receiver, or oven-controlled crystal oscillator)
- **Internal**: REQ-F-HAL-002 (timing interface), REQ-NF-PERF-001 (frequency measurement precision)

**Verification Method**: 
- Laboratory measurement with frequency counter (precision 0.01 ppm minimum)
- 24-hour stability test with temperature logging
- Conformance test suite Grade 1/2 validation scenarios
- Cross-verification with multiple reference sources

---

*[Additional functional requirements REQ-F-DARS-003 through REQ-F-DARS-010 will cover: Capture Range, Phase Relationships, Video Referencing, GPS Referencing, Date/Time Support, Multiple Sampling Frequencies, External Signal Handling, and DARS Distribution]*

---

### 3.2 Synchronization Method Requirements

Requirements for the 4 synchronization methods per AES-11-2009 Section 4.2.

#### REQ-F-SYNC-001: DARS-Referenced Synchronization

- **Trace to**: StR-FUNC-001
- **Priority**: Critical (P0)
- **AES-11 Reference**: Section 4.2.1

**Description**: The system shall support DARS-referenced synchronization where all equipment sample clocks lock to a distributed DARS signal, ensuring phase tolerance within ±5% of AES3 frame period at system outputs.

**Rationale**: DARS-referenced is the preferred method for normal studio practice per AES-11 Section 4.2.1, providing systematic synchronization across multiple equipment.

**Functional Behavior**:
1. System shall accept external DARS signal via dedicated HAL audio interface input
2. System shall lock internal sample clock generator to DARS frequency within capture range (REQ-F-DARS-003)
3. System shall maintain phase relationship within ±5% of AES3 frame period for all outputs (per Section 5.3.1.1)
4. System shall support locking double-rate (96 kHz), quadruple-rate (192 kHz) sampling to basic-rate (48 kHz) DARS
5. System shall maintain lock during DARS signal interruptions up to [TBD] milliseconds (hysteresis)
6. System shall report lock status (UNLOCKED, LOCKING, LOCKED, LOST_LOCK) via diagnostic API
7. System shall measure and report phase offset relative to DARS TRP

**Boundary Values**:
- **Phase Tolerance (Output)**: ±5% of AES3 frame period (per AES-11 Table 2)
  - At 48 kHz: ±1.0 µs (±18° of 360° frame)
  - At 96 kHz: ±0.5 µs (±18° of 360° frame)
- **Lock Acquisition Time**: <2 seconds from cold start (per AES-11 Section 5.2.2 implied)
- **Lock Hysteresis**: Maintain lock through [TBD] ms DARS interruption without sample slip
- **Multi-rate Support**: Lock 96/192 kHz equipment to 48 kHz DARS with correct 2:1 or 4:1 relationship

**Error Handling**:
| Error Condition | System Action | User Notification | Log Level |
|----------------|---------------|-------------------|-----------|
| DARS signal lost | Enter LOST_LOCK state, hold last frequency | `SYNC_WARNING_DARS_LOST` | WARNING |
| Lock acquisition timeout (>5 sec) | Enter UNLOCKED state, free-running | `SYNC_ERROR_LOCK_TIMEOUT` | ERROR |
| Phase drift exceeds ±5% | Trigger re-lock, count violations | `SYNC_WARNING_PHASE_DRIFT` | WARNING |
| Frequency outside capture range | Reject DARS, report out-of-range | `SYNC_ERROR_FREQUENCY_OUT_OF_CAPTURE` | ERROR |

**Acceptance Criteria**:
```gherkin
Scenario: Lock to Grade 1 DARS and maintain phase tolerance
  Given system in UNLOCKED state
  And external Grade 1 DARS at 48 kHz available
  When system enables DARS-referenced synchronization mode
  Then system shall transition to LOCKING state within 100 ms
  And system shall achieve LOCKED state within 2 seconds
  And output TRP phase shall be within ±1.0 µs of DARS TRP
  And lock_status API shall return LOCKED
  And phase_offset_us API shall report actual offset

Scenario: Lock double-rate equipment to basic-rate DARS
  Given 96 kHz sampling equipment
  And external 48 kHz basic-rate DARS available
  When system locks to 48 kHz DARS
  Then system shall generate 96 kHz sample clock (exact 2:1 ratio)
  And output TRP phase at 96 kHz rate shall be within ±0.5 µs tolerance
  And every 2nd TRP shall align with 48 kHz DARS TRP ±1.0 µs

Scenario: Maintain lock through brief DARS interruption
  Given system in LOCKED state to DARS
  And lock established for >10 seconds
  When DARS signal interrupted for [TBD] milliseconds
  Then system shall maintain LOCKED state
  And system shall hold last frequency using internal oscillator
  And system shall NOT introduce sample slips
  When DARS signal returns
  Then system shall re-synchronize phase within 500 ms
  And lock_status shall remain LOCKED throughout

Scenario: Handle DARS frequency outside capture range
  Given Grade 1 equipment with ±2 ppm capture range
  And external DARS at 48.15 kHz (+3125 ppm, exceeds ±2 ppm)
  When system attempts DARS-referenced synchronization
  Then system shall reject DARS
  And system shall log ERROR with code `SYNC_ERROR_FREQUENCY_OUT_OF_CAPTURE`
  And system shall remain in free-running UNLOCKED state
  And lock_status API shall return UNLOCKED
```

**Dependencies**:
- **Internal**: REQ-F-DARS-001 (DARS format), REQ-F-DARS-002 (frequency accuracy), REQ-F-DARS-003 (capture range), REQ-F-HAL-001 (audio interface), REQ-NF-PERF-003 (phase tolerance)

**Verification Method**: 
- Lock acquisition timing measurement with oscilloscope
- Phase relationship verification with dual-channel analyzer
- Multi-rate lock validation (48→96→192 kHz cascades)
- DARS interruption resilience testing

---

#### REQ-F-DARS-003: Capture Range Support

- **Trace to**: StR-PERF-002, StR-FUNC-001
- **Priority**: Critical (P0)
- **AES-11 Reference**: Section 5.2.2

**Description**: The system shall lock internal oscillators to external DARS inputs within specified capture ranges: ±2 ppm for Grade 1 equipment, ±50 ppm for Grade 2 equipment.

**Rationale**: AES-11-2009 Section 5.2.2 defines minimum capture ranges ensuring equipment can lock to reference signals despite manufacturing tolerances and environmental variations.

**Functional Behavior**:
1. System shall detect external DARS frequency within measurement precision of ±0.01 ppm
2. When configured as Grade 1, system shall lock to DARS within ±2 ppm of nominal frequency
3. When configured as Grade 2, system shall lock to DARS within ±50 ppm of nominal frequency
4. System shall acquire lock within 2 seconds when frequency is within capture range
5. System shall reject DARS exceeding capture range and remain in free-running mode
6. System shall measure actual capture range via diagnostic API for verification
7. System shall support capture range testing mode for conformance validation

**Boundary Values**:
- **Grade 1 Capture Range**: ±2 ppm
  - At 48 kHz: 47.9999040 Hz to 48.0000960 Hz (±0.096 Hz)
  - At 44.1 kHz: 44.0999118 Hz to 44.1000882 Hz (±0.0882 Hz)
- **Grade 2 Capture Range**: ±50 ppm
  - At 48 kHz: 47.9976 Hz to 48.0024 Hz (±2.4 Hz)
  - At 44.1 kHz: 44.09779 Hz to 44.10221 Hz (±2.205 Hz)
- **Lock Acquisition Time**: <2 seconds (target 1 second typical)
- **Frequency Measurement Precision**: ±0.01 ppm minimum

**Error Handling**:
| Error Condition | System Action | User Notification | Log Level |
|----------------|---------------|-------------------|-----------|
| DARS frequency outside capture range | Reject DARS, free-running mode | `SYNC_ERROR_OUTSIDE_CAPTURE_RANGE` | ERROR |
| Lock acquisition timeout | Enter UNLOCKED state | `SYNC_ERROR_LOCK_TIMEOUT` | ERROR |
| Frequency measurement uncertainty high | Log warning, continue attempt | `SYNC_WARNING_MEASUREMENT_UNCERTAINTY` | WARNING |
| Multiple lock/unlock cycles | Enter error state after 5 cycles | `SYNC_ERROR_UNSTABLE_LOCK` | CRITICAL |

**Acceptance Criteria**:
```gherkin
Scenario: Grade 1 equipment locks to DARS within ±2 ppm
  Given Grade 1 equipment with 48 kHz nominal frequency
  And external DARS at 48.00008 Hz (+1.67 ppm, within ±2 ppm)
  When system attempts lock acquisition
  Then system shall achieve LOCKED state within 2 seconds
  And frequency_error_ppm API shall report +1.67 ppm
  And lock_status shall return LOCKED

Scenario: Grade 1 equipment rejects DARS outside ±2 ppm
  Given Grade 1 equipment with 48 kHz nominal frequency
  And external DARS at 48.00015 Hz (+3.125 ppm, exceeds ±2 ppm)
  When system attempts lock acquisition
  Then system shall reject DARS
  And system shall log ERROR `SYNC_ERROR_OUTSIDE_CAPTURE_RANGE`
  And lock_status shall return UNLOCKED
  And system shall remain in free-running mode

Scenario: Grade 2 equipment locks to DARS within ±50 ppm
  Given Grade 2 equipment with 48 kHz nominal frequency
  And external DARS at 48.002 Hz (+41.67 ppm, within ±50 ppm)
  When system attempts lock acquisition
  Then system shall achieve LOCKED state within 2 seconds
  And frequency_error_ppm API shall report +41.67 ppm
  And lock_status shall return LOCKED

Scenario: Lock acquisition time measurement
  Given system in UNLOCKED state
  And external DARS within capture range
  And timestamp T0 when lock acquisition begins
  When system initiates synchronization
  Then system shall achieve LOCKED state at timestamp T1
  And (T1 - T0) shall be <2000 milliseconds
  And lock_acquisition_time_ms API shall report actual time
```

**Dependencies**:
- **Internal**: REQ-F-DARS-001 (DARS format), REQ-F-DARS-002 (frequency accuracy), REQ-F-HAL-002 (timing interface)
- **External**: High-precision frequency measurement capability via HAL

**Verification Method**:
- Frequency offset testing with calibrated signal generator
- Lock acquisition timing measurement with microsecond precision
- Boundary testing at ±1.9 ppm, ±2.1 ppm (Grade 1) and ±49 ppm, ±51 ppm (Grade 2)
- Conformance test automation

---

#### REQ-F-DARS-004: Phase Relationship Tolerances

- **Trace to**: StR-PERF-003, StR-FUNC-001
- **Priority**: Critical (P0)
- **AES-11 Reference**: Section 5.3.1, Table 2

**Description**: The system shall maintain timing reference point (TRP) phase relationships within specified tolerances: outputs ±5% of AES3 frame period, inputs ±25% of AES3 frame period.

**Rationale**: AES-11-2009 Section 5.3.1 defines phase tolerances ensuring equipment synchronization without sample slips, while Table 2 quantifies exact microsecond limits for each sampling frequency.

**Functional Behavior**:
1. System shall measure TRP phase offset between DARS input and all output signals
2. System shall maintain output TRP phase within ±5% (±18°) of DARS TRP at equipment connectors
3. System shall accept input signals with TRP phase within ±25% (±90°) of DARS TRP without sample slips
4. System shall introduce known constant device delay (document in microseconds or frame periods)
5. System shall provide phase offset measurement via diagnostic API (in microseconds and degrees)
6. System shall trigger phase realignment when offset exceeds ±4.5% (warning threshold)
7. System shall count and report phase violation events for monitoring

**Boundary Values** (from AES-11 Table 2):
- **32 kHz Sampling**:
  - Frame period: 31.25 µs (1/32000)
  - Output tolerance: ±1.6 µs (±5% = ±18°)
  - Input tolerance: ±7.8 µs (±25% = ±90°)
- **44.1 kHz Sampling**:
  - Frame period: 22.68 µs (1/44100)
  - Output tolerance: ±1.1 µs (±5% = ±18°)
  - Input tolerance: ±5.7 µs (±25% = ±90°)
- **48 kHz Sampling**:
  - Frame period: 20.83 µs (1/48000)
  - Output tolerance: ±1.0 µs (±5% = ±18°)
  - Input tolerance: ±5.2 µs (±25% = ±90°)
- **96 kHz Sampling** (double-rate):
  - Frame period: 10.41 µs (1/96000)
  - Output tolerance: ±0.5 µs (±5% = ±18°)
  - Input tolerance: ±2.6 µs (±25% = ±90°)

**Error Handling**:
| Error Condition | System Action | User Notification | Log Level |
|----------------|---------------|-------------------|-----------|
| Output phase exceeds ±5% | Trigger realignment, count violation | `SYNC_WARNING_OUTPUT_PHASE_VIOLATION` | WARNING |
| Input phase exceeds ±25% | Accept with hysteresis, log event | `SYNC_INFO_INPUT_PHASE_LIMIT` | INFO |
| Phase measurement failure | Use last known value, flag uncertainty | `SYNC_ERROR_PHASE_MEASUREMENT_FAIL` | ERROR |
| Realignment failure after 3 attempts | Enter error state, require restart | `SYNC_ERROR_REALIGNMENT_FAIL` | CRITICAL |

**Acceptance Criteria**:
```gherkin
Scenario: Maintain output phase within ±5% at 48 kHz
  Given system locked to DARS at 48 kHz
  And DARS TRP at reference time T0
  When system generates output signal TRP at time T1
  Then |T1 - T0| shall be ≤1.0 µs (±5% of 20.83 µs frame)
  And phase_offset_us API shall report actual offset
  And phase_offset_degrees API shall report ±18° maximum

Scenario: Accept input signal with phase within ±25% at 48 kHz
  Given system locked to DARS at 48 kHz
  And input signal TRP offset by +5.0 µs from DARS TRP
  And +5.0 µs is within ±5.2 µs tolerance (±25%)
  When system receives input signal
  Then system shall accept signal without sample slip
  And device_delay_samples API shall report constant delay
  And no phase violation events shall be logged

Scenario: Handle output phase violation and realignment
  Given system locked to DARS
  And output phase drifts to +1.2 µs at 48 kHz (exceeds ±1.0 µs)
  When system detects phase violation
  Then system shall log WARNING `SYNC_WARNING_OUTPUT_PHASE_VIOLATION`
  And system shall trigger phase realignment
  And system shall achieve ≤1.0 µs phase offset within 1 second
  And phase_violation_count shall increment

Scenario: Phase measurement across sampling frequencies
  Given system supports 32/44.1/48/96 kHz sampling
  When system measures phase at each frequency
  Then tolerance at 32 kHz shall be ±1.6 µs output, ±7.8 µs input
  And tolerance at 44.1 kHz shall be ±1.1 µs output, ±5.7 µs input
  And tolerance at 48 kHz shall be ±1.0 µs output, ±5.2 µs input
  And tolerance at 96 kHz shall be ±0.5 µs output, ±2.6 µs input
  And phase measurement precision shall be ≤0.1 µs
```

**Dependencies**:
- **Internal**: REQ-F-DARS-001 (TRP extraction), REQ-F-HAL-002 (microsecond timing), REQ-NF-PERF-001 (timing precision)
- **External**: AES3-2009 for preamble timing definitions

**Verification Method**:
- Dual-channel oscilloscope measurement of TRP timing
- Phase offset sweep testing across full ±25% input range
- Automated phase violation detection and logging verification
- Cross-frequency validation (32/44.1/48/96 kHz)

---

#### REQ-F-DARS-005: Video-Referenced Synchronization

- **Trace to**: StR-FUNC-001
- **Priority**: High (P1)
- **AES-11 Reference**: Section 4.2.3, 4.5, 5.3.4

**Description**: The system shall support video-referenced synchronization where DARS is derived from video reference signal, aligning DARS TRP to half-amplitude point of video sync pulse leading edge with ±5% tolerance.

**Rationale**: AES-11-2009 Section 4.5 enables audio-video synchronization in combined production environments, maintaining mathematical relationships from Table 1 for all video frame rates including NTSC pull-down.

**Functional Behavior**:
1. System shall accept video reference signal via HAL video sync interface
2. System shall detect video sync pulse half-amplitude point on line 1 (525-line) or line 4 (625-line)
3. System shall generate DARS with X/Z preamble aligned to video sync pulse per Section 5.3.4
4. System shall maintain ±5% phase tolerance between DARS and video reference (Section 5.3.5)
5. System shall support integer ratio video systems (Table 1: 24, 25, 30 Hz and multiples)
6. System shall handle non-integer ratio NTSC systems (30/1.001 Hz) with ±1 sample offset tolerance
7. System shall calculate samples per video frame per Table 1 relationships
8. System shall identify alignment frame for non-integer ratios (every 5th frame for NTSC)

**Boundary Values** (from AES-11 Table 1):
- **Integer Ratios (Synchronous Lock)**:
  - 48 kHz @ 24 Hz video: 2000 samples/frame (exact)
  - 48 kHz @ 25 Hz video: 1920 samples/frame (exact)
  - 48 kHz @ 30 Hz video: 1600 samples/frame (exact)
  - 44.1 kHz @ 25 Hz video: 1764 samples/frame (exact)
- **Non-Integer Ratios (±1 sample tolerance)**:
  - 48 kHz @ 30/1.001 Hz (29.97 Hz NTSC): 1601.6 samples/frame (~1601 or 1602)
  - 44.1 kHz @ 30/1.001 Hz: 1471.47 samples/frame (~1471 or 1472)
- **Phase Tolerance**: ±5% of AES3 frame period for video-to-DARS alignment (Section 5.3.5)
- **Video Line Reference**: Line 1 (525/59.94 systems) or Line 4 (625/50 systems)

**Error Handling**:
| Error Condition | System Action | User Notification | Log Level |
|----------------|---------------|-------------------|-----------|
| Video sync signal lost | Hold last DARS frequency, flag warning | `VIDEO_SYNC_WARNING_SIGNAL_LOST` | WARNING |
| Video frame rate unsupported | Reject video reference, use alternate method | `VIDEO_SYNC_ERROR_UNSUPPORTED_RATE` | ERROR |
| Sample count mismatch (non-integer) | Accept ±1 sample variance, log event | `VIDEO_SYNC_INFO_NON_INTEGER_RATIO` | INFO |
| Video-to-DARS phase exceeds ±5% | Trigger realignment | `VIDEO_SYNC_WARNING_PHASE_VIOLATION` | WARNING |

**Acceptance Criteria**:
```gherkin
Scenario: Video-referenced DARS at 48 kHz / 25 Hz (integer ratio)
  Given 25 Hz video reference signal (PAL)
  And system configured for 48 kHz audio sampling
  When system derives DARS from video reference
  Then DARS shall have exactly 1920 samples per video frame (48000/25)
  And DARS X preamble shall align to video line 1 sync half-amplitude point
  And alignment shall be within ±1.0 µs (±5% of 20.83 µs frame @ 48 kHz)
  And synchronization shall be maintained on every video frame

Scenario: Video-referenced DARS at 48 kHz / 29.97 Hz (non-integer NTSC)
  Given 29.97 Hz video reference (30/1.001 Hz NTSC)
  And system configured for 48 kHz audio sampling
  When system derives DARS from video reference
  Then DARS shall have 1601 or 1602 samples per video frame (±1 sample tolerance)
  And alignment frame shall occur every 5th video frame (per AES-11 Section 4.5.3)
  And system shall provide sufficient hysteresis to prevent sample slips
  And video_audio_ratio API shall report 1601.6 samples/frame (non-integer)

Scenario: Handle video sync signal loss
  Given system locked to video reference
  And stable DARS generation for >10 seconds
  When video sync signal is lost
  Then system shall log WARNING `VIDEO_SYNC_WARNING_SIGNAL_LOST`
  And system shall hold last DARS frequency using internal oscillator
  And system shall maintain synchronization for [TBD] seconds (holdover)
  When video sync signal returns
  Then system shall re-synchronize within 2 seconds

Scenario: Video frame rate validation
  Given system receives video reference signal
  When system detects video frame rate
  Then system shall validate rate against Table 1 supported rates
  And system shall calculate exact samples per frame relationship
  And system shall identify integer vs non-integer ratio
  And video_frame_rate_hz API shall report detected rate
  And samples_per_video_frame API shall report calculated samples
```

**Dependencies**:
- **Internal**: REQ-F-DARS-001 (DARS generation), REQ-F-HAL-003 (video sync interface), REQ-F-DARS-004 (phase tolerance)
- **External**: SMPTE 318M-1999 for video sync specifications

**Verification Method**:
- Video sync pulse measurement with oscilloscope
- Sample count verification across multiple video frames
- NTSC non-integer ratio testing with ±1 sample variance validation
- Interoperability testing with video equipment

---

#### REQ-F-DARS-006: GPS-Referenced Synchronization

- **Trace to**: StR-FUNC-001
- **Priority**: Medium (P2)
- **AES-11 Reference**: Section 4.2.4

**Description**: The system shall support GPS-referenced synchronization using GPS receiver to provide frequency reference, phase alignment from 1-second pulses, and time-of-day sample address code.

**Rationale**: AES-11-2009 Section 4.2.4 enables GPS-based synchronization for distributed facilities requiring common time reference without direct signal distribution.

**Functional Behavior**:
1. System shall accept GPS 1-pulse-per-second (1PPS) timing signal via HAL GPS interface
2. System shall lock DARS frequency to GPS-derived 10 MHz reference (if available)
3. System shall align DARS phase to GPS 1PPS rising edge within ±1 µs
4. System shall extract GPS time-of-day and encode in AES3 channel status bytes 18-21
5. System shall support GPS signal loss with holdover mode (maintain last frequency)
6. System shall report GPS lock status (UNLOCKED, ACQUIRING, LOCKED, HOLDOVER)
7. System shall measure GPS timing jitter and report via diagnostic API

**Boundary Values**:
- **1PPS Timing Accuracy**: ±1 µs phase alignment to GPS 1PPS edge
- **Frequency Reference**: 10 MHz from GPS disciplined oscillator (if available)
- **Holdover Duration**: Maintain ±1 ppm accuracy for minimum 24 hours after GPS loss
- **Time-of-Day Encoding**: UTC time in AES3 channel status per AES-11 Section 5.1.5
- **GPS Acquisition Time**: <5 minutes for cold start, <30 seconds for warm start

**Error Handling**:
| Error Condition | System Action | User Notification | Log Level |
|----------------|---------------|-------------------|-----------|
| GPS signal lost | Enter HOLDOVER mode, use internal TCXO | `GPS_WARNING_SIGNAL_LOST` | WARNING |
| GPS never acquires (timeout) | Fall back to alternate sync method | `GPS_ERROR_ACQUISITION_TIMEOUT` | ERROR |
| GPS time-of-day invalid | Skip time encoding, sync frequency only | `GPS_WARNING_INVALID_TIME` | WARNING |
| Holdover exceeds 24 hours | Flag degraded accuracy | `GPS_WARNING_HOLDOVER_EXPIRED` | WARNING |

**Acceptance Criteria**:
```gherkin
Scenario: GPS-referenced DARS with 1PPS phase lock
  Given GPS receiver with valid 1PPS signal
  And GPS lock status is LOCKED
  When system generates DARS aligned to GPS 1PPS
  Then DARS X preamble shall align to 1PPS rising edge within ±1 µs
  And gps_lock_status API shall return LOCKED
  And gps_phase_offset_ns API shall report <1000 ns offset

Scenario: GPS time-of-day encoding in channel status
  Given GPS receiver with valid time-of-day (2025-11-07 14:30:00 UTC)
  When system encodes time in AES3 channel status
  Then bytes 18-21 shall contain UTC time per AES-11 Section 5.1.5
  And time_of_day API shall return "2025-11-07T14:30:00Z"
  And channel status metadata flag shall be set per AES3

Scenario: GPS holdover mode after signal loss
  Given system in GPS LOCKED state for >1 hour
  And GPS holdover uses TCXO with ±0.5 ppm stability
  When GPS signal is lost
  Then system shall enter HOLDOVER mode within 1 second
  And system shall log WARNING `GPS_WARNING_SIGNAL_LOST`
  And frequency accuracy shall remain ±1 ppm for 24 hours
  And gps_lock_status API shall return HOLDOVER
  And holdover_duration_seconds API shall increment

Scenario: GPS acquisition timeout and fallback
  Given system configured for GPS-referenced sync
  And no GPS signal available after 10 minutes
  When GPS acquisition timeout occurs
  Then system shall log ERROR `GPS_ERROR_ACQUISITION_TIMEOUT`
  And system shall fall back to DARS-referenced sync (if available)
  And gps_lock_status API shall return UNLOCKED
```

**Dependencies**:
- **Internal**: REQ-F-DARS-001 (DARS generation), REQ-F-HAL-004 (GPS interface)
- **External**: IEEE 1588-2019 repository (optional PTP support), GPS receiver hardware

**Verification Method**:
- GPS simulator testing with controlled 1PPS and time-of-day signals
- Holdover accuracy measurement over 24+ hours
- Time-of-day encoding validation
- Signal loss and recovery testing

---

### 3.3 Hardware Abstraction Layer (HAL) Requirements

Requirements for platform-independent hardware interfaces per StR-FUNC-002.

#### REQ-F-HAL-001: Audio Interface Abstraction

- **Trace to**: StR-FUNC-002, StR-USER-001
- **Priority**: Critical (P0)

**Description**: The system shall provide hardware abstraction layer for audio interfaces enabling platform-independent DARS transmission and reception without vendor-specific code in protocol implementation.

**Rationale**: Hardware abstraction is core architectural requirement enabling "compile without any vendor drivers" principle and supporting ARM Cortex-M7, x86-64 Linux/Windows platforms.

**Functional Behavior**:
1. System shall define `audio_hal_t` interface structure with function pointers for audio operations
2. System shall provide `send_audio_frame()` function to transmit AES3 frames
3. System shall provide `receive_audio_frame()` function to receive AES3 frames
4. System shall provide `set_sample_rate()` function for 32/44.1/48/96 kHz configuration
5. System shall provide `get_audio_status()` function for interface health monitoring
6. System shall support asynchronous callback model for real-time audio processing
7. System shall document HAL contract in header file with usage examples

**Interface Definition**:
```c
typedef struct audio_hal {
    /* Send AES3 frame to hardware (non-blocking) */
    int (*send_audio_frame)(const aes3_frame_t* frame);
    
    /* Receive AES3 frame from hardware (non-blocking) */
    int (*receive_audio_frame)(aes3_frame_t* frame);
    
    /* Set sampling frequency (32000, 44100, 48000, 96000 Hz) */
    int (*set_sample_rate)(uint32_t sample_rate_hz);
    
    /* Get audio interface status */
    int (*get_audio_status)(audio_status_t* status);
    
    /* Register callback for received frames (optional) */
    int (*register_rx_callback)(audio_frame_callback_t callback, void* user_data);
    
    /* Hardware-specific context (opaque to protocol) */
    void* hw_context;
} audio_hal_t;
```

**Boundary Values**:
- **Supported Sample Rates**: 32000, 44100, 48000, 96000 Hz (per AES5-2018)
- **Frame Buffer Size**: 192 bits (24 bytes) for AES3 stereo frame
- **Callback Latency**: <10 µs from frame arrival to callback invocation (embedded targets)
- **Error Return Codes**: 0 = success, negative = error code

**Error Handling**:
| Error Condition | System Action | User Notification | Log Level |
|----------------|---------------|-------------------|-----------|
| HAL function pointer NULL | Return `-EINVAL`, log error | `HAL_ERROR_NULL_FUNCTION` | CRITICAL |
| Unsupported sample rate | Return `-ENOTSUP` | `HAL_ERROR_UNSUPPORTED_RATE` | ERROR |
| Hardware unavailable | Return `-ENODEV` | `HAL_ERROR_NO_DEVICE` | ERROR |
| Frame buffer overflow | Drop frame, increment counter | `HAL_WARNING_BUFFER_OVERFLOW` | WARNING |

**Acceptance Criteria**:
```gherkin
Scenario: Initialize audio HAL with platform implementation
  Given ASIO platform audio implementation
  And ASIO provides functions matching audio_hal_t interface
  When protocol layer calls aes11_init(&asio_audio_hal)
  Then protocol shall store HAL pointer
  And protocol shall NOT call any ASIO-specific functions directly
  And protocol shall use only HAL interface for audio operations

Scenario: Send DARS frame via HAL
  Given protocol has valid audio_hal_t interface
  And DARS frame with 48 kHz sampling configured
  When protocol calls hal->send_audio_frame(&dars_frame)
  Then HAL shall transmit frame to hardware
  And HAL shall return 0 on success
  And frame shall appear on physical AES3 output within 1 sample period

Scenario: Receive external DARS via HAL callback
  Given protocol registered rx_callback with HAL
  When hardware receives AES3 frame
  Then HAL shall invoke rx_callback within <10 µs
  And callback shall receive valid aes3_frame_t pointer
  And protocol shall extract TRP and process DARS

Scenario: Handle unsupported sample rate
  Given audio HAL implementation
  When protocol calls hal->set_sample_rate(88200)  // Not in AES5
  Then HAL shall return -ENOTSUP
  And protocol shall log ERROR `HAL_ERROR_UNSUPPORTED_RATE`
  And sample rate shall remain at previous valid setting
```

**Dependencies**:
- **External**: Platform-specific HAL implementations (ASIO, ALSA, CoreAudio bridges in service layer)
- **Internal**: REQ-F-DARS-001 (AES3 frame structure)

**Verification Method**:
- HAL validator tool testing interface contract
- Mock HAL implementation for unit testing
- Multi-platform HAL implementation verification (ARM, x86 Linux, x86 Windows)

---

#### REQ-F-HAL-002: Timing Interface Abstraction

- **Trace to**: StR-FUNC-002, StR-PERF-001
- **Priority**: Critical (P0)

**Description**: The system shall provide hardware abstraction for high-precision timing interfaces enabling microsecond-resolution time measurements without platform-specific timing code in protocol implementation.

**Rationale**: Timing precision is critical for AES-11 synchronization (Grade 1: ±1 ppm requires <1 ns jitter). HAL abstraction enables platform-independent protocol with platform-specific optimizations (e.g., RDTSC on x86, DWT_CYCCNT on ARM).

**Functional Behavior**:
1. System shall define `timing_hal_t` interface structure for timing operations
2. System shall provide `get_time_ns()` function returning monotonic nanosecond timestamp
3. System shall provide `get_sample_time()` function for audio sample-accurate timing
4. System shall provide `sleep_us()` function for microsecond-precision delays
5. System shall provide timing resolution query via `get_timing_resolution()`
6. System shall support timing reference synchronization via `sync_timing_reference()`
7. System shall guarantee monotonic time (no backwards jumps)

**Interface Definition**:
```c
typedef struct timing_hal {
    /* Get monotonic timestamp in nanoseconds */
    uint64_t (*get_time_ns)(void);
    
    /* Get sample-accurate timestamp (sample number since epoch) */
    uint64_t (*get_sample_time)(uint32_t sample_rate_hz);
    
    /* Sleep for specified microseconds (blocking) */
    int (*sleep_us)(uint32_t microseconds);
    
    /* Get timing resolution in nanoseconds */
    uint32_t (*get_timing_resolution_ns)(void);
    
    /* Synchronize timing reference (e.g., GPS 1PPS) */
    int (*sync_timing_reference)(uint64_t reference_time_ns);
    
    /* Hardware-specific context */
    void* hw_context;
} timing_hal_t;
```

**Boundary Values**:
- **Timing Resolution**: ≤100 ns (target 10 ns for Grade 1)
- **Timestamp Range**: 64-bit unsigned (0 to 2^64-1 nanoseconds = 584 years)
- **Monotonic Guarantee**: Timestamps never decrease
- **Sleep Accuracy**: ±10 µs for sleep_us() calls

**Error Handling**:
| Error Condition | System Action | User Notification | Log Level |
|----------------|---------------|-------------------|-----------|
| Timing resolution insufficient | Log warning, use best available | `TIMING_WARNING_LOW_RESOLUTION` | WARNING |
| Timestamp overflow (unlikely) | Wrap at 64-bit limit | `TIMING_INFO_TIMESTAMP_WRAP` | INFO |
| Timing reference sync failure | Return error code | `TIMING_ERROR_SYNC_FAIL` | ERROR |

**Acceptance Criteria**:
```gherkin
Scenario: Measure TRP phase offset with nanosecond precision
  Given timing HAL with ≤100 ns resolution
  And DARS TRP occurs at hardware timestamp T_dars
  And output TRP occurs at hardware timestamp T_output
  When protocol calls hal->get_time_ns() for both TRPs
  Then phase_offset_ns = |T_output - T_dars|
  And phase offset measurement precision shall be ≤100 ns
  And measurement shall enable ±1.0 µs tolerance validation @ 48 kHz

Scenario: Monotonic timestamp guarantee
  Given timing HAL implementation
  When protocol calls hal->get_time_ns() repeatedly (1000 iterations)
  Then every timestamp T[i+1] shall be ≥ T[i]
  And no backwards time jumps shall occur
  And timestamps shall increment monotonically

Scenario: Sample-accurate timing for audio synchronization
  Given 48 kHz sampling frequency
  And system started at T0 nanoseconds
  When protocol calls hal->get_sample_time(48000) at T1 nanoseconds
  Then returned sample_number = (T1 - T0) * 48000 / 1e9
  And sample timing shall be accurate to ±1 sample

Scenario: Timing resolution query
  Given platform timing HAL (e.g., ARM DWT_CYCCNT @ 216 MHz)
  When protocol calls hal->get_timing_resolution_ns()
  Then HAL shall return resolution = 1e9 / 216e6 ≈ 4.6 ns
  And protocol shall verify resolution ≤100 ns requirement
```

**Dependencies**:
- **External**: Platform timing sources (RDTSC, DWT_CYCCNT, clock_gettime, QueryPerformanceCounter)
- **Internal**: REQ-F-DARS-004 (phase measurement), REQ-NF-PERF-001 (timing precision)

**Verification Method**:
- Timing resolution measurement on target platforms
- Monotonicity stress testing (1M consecutive calls)
- Phase measurement accuracy validation with oscilloscope
- Cross-platform timing comparison

---

*[Continuing with REQ-F-HAL-003 (Sync Interface), REQ-F-HAL-004 (GPIO/Video/GPS), and remaining functional requirements...]*

---

## Status: Phase 02 In Progress

✅ **Completed**:
- SyRS document structure (ISO/IEC/IEEE 29148:2018 compliant)
- Section 1: Introduction (Purpose, Scope, Definitions, References, Overview)
- Section 2: System Overview (Context, Capabilities, User Classes, Operating Environment, Assumptions/Dependencies)
- Section 3.1: DARS Protocol Requirements (REQ-F-DARS-001, REQ-F-DARS-002 detailed with 8-dimension elicitation)
- Section 3.2: Synchronization Method Requirements (REQ-F-SYNC-001 detailed)

⏳ **In Progress**:
- Expanding functional requirements with complete elicitation framework application
- Creating non-functional requirements from stakeholder NFRs

📋 **Next Steps**:
1. Complete all functional requirements (REQ-F-xxx) with 8-dimension analysis
2. Develop non-functional requirements (REQ-NF-xxx) with metrics and test methods
3. Create use cases (UC-xxx) for key scenarios
4. Develop user stories (STORY-xxx) with Given-When-Then acceptance criteria
5. Build complete traceability matrix (StR → REQ → UC → STORY)

---

**Document Generation Note**: This is a living document following Agile/XP principles. Requirements will evolve as implementation feedback is gathered. All changes maintain bidirectional traceability to stakeholder requirements and AES-11-2009 specification sections.
