# Stakeholder Requirements - Compliance Requirements

**Part of**: Stakeholder Requirements Specification (StRS-AES11-001)  
**Section**: 3.5 Compliance Requirements (Standards and Legal)

---

## StR-COMP-001: AES-11-2009 Standards Compliance

**ID**: StR-COMP-001  
**Priority**: Critical (P0)  
**Category**: Standards Compliance  
**Source**: AES Standards Body, Audio Equipment Manufacturers  

**Stakeholder Statement**:
> "We need 100% compliance with AES-11-2009 standard so our products can achieve AES certification and interoperate reliably with other manufacturers' equipment."

**Requirement**:
The solution shall achieve 100% compliance with AES-11-2009 (reaffirmed 2014) "AES recommended practice for digital audio engineering - Synchronization of digital audio equipment in studio operations" as validated by comprehensive conformance testing and AES Technical Council review.

**Rationale**:
- AES-11-2009 is normative standard for digital audio synchronization
- Certification readiness is critical business requirement (StR-BUS-001)
- Standards compliance ensures multi-vendor interoperability
- Deviations from standard prevent certification and market acceptance

**Compliance Scope (AES-11-2009 Sections)**:

| Section | Title | Compliance Requirement |
|---------|-------|------------------------|
| **Section 1** | Scope | Implementation covers all areas of application per 1.2 |
| **Section 2** | Normative References | Depends on AES3, AES5 per specifications |
| **Section 3** | Definitions | Uses exact definitions (synchronism, TRP, DARS) |
| **Section 4** | Modes of Operation | Implements all four synchronization methods |
| **Section 5** | Equipment Synchronization | Meets all DARS, timing, and system requirements |
| **Section 6** | Clock Specifications | ADC/DAC clocks meet enhanced precision |
| **Section 7** | Date and Time | Optional but supported when implemented |
| **Annexes** | Informative | Guidance followed, not mandatory |

**Success Criteria**:
- ✅ 100% conformance to AES-11-2009 normative sections (1-7)
- ✅ Conformance test suite validates all requirements
- ✅ AES Technical Council review approval (reference implementation status)
- ✅ No deviations or exceptions from standard requirements
- ✅ Conformance report documents compliance evidence

**Acceptance Criteria**:
```gherkin
Feature: AES-11-2009 normative compliance
  As an AES standards body
  I want 100% compliance with AES-11-2009
  So that implementation is certifiable

  Scenario: Section 1 Scope compliance
    Given implementation covers professional digital audio
    Then studio environment interconnections supported (1.2.1)
    And external source/destination interconnections supported (1.2.2)
    And video-associated audio supported (1.2.3)
    And all areas of application documented

  Scenario: Section 3 Definitions compliance
    Given AES-11-2009 definitions
    Then synchronism implemented per 3.1 (frame frequency and phase identical)
    And AES3 frame structure used per 3.2
    And timing reference point (TRP) per 3.3 (X/Z preamble transition)
    And DARS conforms to definition per 3.4

  Scenario: Section 4 Modes of Operation compliance
    Given all four synchronization methods
    Then DARS-referenced mode implemented per 4.2.1
    And audio input-referenced mode implemented per 4.2.2
    And video-referenced mode implemented per 4.2.3
    And GPS-referenced mode implemented per 4.2.4
    And DARS distribution conforms to AES3 per 4.3

  Scenario: Section 5 Synchronization Practice compliance
    Given DARS implementation
    Then DARS requirements met per 5.1 (format, grade, identification)
    And frequency tolerances met per 5.2 (Grade 1: ±1 ppm, Grade 2: ±10 ppm)
    And capture ranges met per 5.2.2 (Grade 1: ±2 ppm, Grade 2: ±50 ppm)
    And timing relationships met per 5.3 (output ±5%, input ±25%)
    And video referencing per 5.3.4 when implemented

  Scenario: Section 6 Clock Specifications compliance
    Given ADC/DAC sampling clocks
    Then timing precision exceeds DARS distribution per 6.1
    And jitter performance meets enhanced requirements

  Scenario: Conformance validation
    Given complete implementation
    When conformance test suite executed
    Then all Section 5 requirements pass
    And all Section 6 requirements pass
    And conformance report generated
    And no deviations from standard documented
```

**Dependencies**:
- StR-FUNC-001 (Core DARS protocol implementing all sections)
- StR-FUNC-003 (Conformance testing suite)
- StR-PERF-001 through StR-PERF-004 (All performance requirements)
- External: AES-11-2009 specification document access for validation

**Risks**:
- **Risk**: Standard interpretation ambiguity → **Mitigation**: AES Technical Council consultation
- **Risk**: Hidden non-compliance discovered late → **Mitigation**: Early conformance testing, iterative validation

**Validation Method**: Automated conformance test suite, AES Technical Council review, third-party certification testing

---

## StR-COMP-002: AES3-2009 Digital Audio Interface Compliance

**ID**: StR-COMP-002  
**Priority**: Critical (P0)  
**Category**: Standards Integration  
**Source**: AES-11-2009 Section 2 (Normative References)  

**Stakeholder Statement**:
> "AES-11 DARS signals must conform to AES3 digital audio interface format for electrical compatibility with professional audio equipment."

**Requirement**:
The solution shall use AES3-2009 "AES recommended practice for digital audio engineering - serial transmission format for linearly represented digital audio data" for DARS signal format, channel status encoding, and electrical characteristics per AES-11-2009 Section 2 normative reference.

**Rationale**:
- AES-11-2009 Section 4.3: "DARS shall be distributed in compliance with AES3"
- AES-11-2009 Section 5.1.1: "DARS shall have format and electrical configuration of two-channel digital audio interface"
- AES3 compliance ensures physical interconnection compatibility
- Channel status structure used for DARS identification (byte 4)

**AES3-2009 Compliance Requirements**:

**Frame Format** (AES3-1-2009):
- Two subframes per frame (channels A and B)
- Preambles X, Y, Z mark frame and subframe boundaries
- 32 bits per subframe (4 control bits + 28 data bits)
- Bi-phase mark coding (BMC) for self-clocking

**Channel Status** (AES3-2-2009):
- 192-bit channel status structure
- Byte 4 bits 0-1 for DARS grade identification (AES-11 Section 5.1.3)
- "Non-audio" flag when DARS contains non-PCM data (Section 5.1.4)
- Metadata flags for date/time in user channel (Section 5.1.5)

**Electrical Specifications**:
- Same connector as AES3 (typically XLR-3)
- Voltage levels and impedance per AES3
- Cable requirements per AES3

**Success Criteria**:
- ✅ DARS frames conform to AES3-1-2009 frame structure
- ✅ Channel status encoding per AES3-2-2009
- ✅ Preambles (X, Y, Z) conform to AES3-3-2009
- ✅ Jitter tolerance meets AES3-4-2009 specifications
- ✅ Electrical characteristics per AES3 (when implemented in hardware)
- ✅ Integration with external AES3-2009 repository validated

**Acceptance Criteria**:
```gherkin
Feature: AES3-2009 frame format compliance
  As DARS implementation
  I want to use AES3 frame format
  So that DARS is electrically compatible

  Scenario: Frame structure per AES3-1-2009
    Given DARS signal generation
    Then frame consists of two subframes (A and B)
    And each subframe has 32 bits
    And preambles X, Y, Z mark boundaries correctly
    And bi-phase mark coding (BMC) is used
    And frame structure validated by AES3 parser

  Scenario: Channel status per AES3-2-2009
    Given DARS channel status encoding
    Then 192-bit channel status structure used
    And byte 0 bit 0 indicates consumer/professional (professional = 0)
    And byte 4 bits 0-1 encode DARS grade (01 = Grade 1, 10 = Grade 2)
    And byte 0 bit 1 indicates audio/non-audio per Section 5.1.4
    And channel status validated by AES3 decoder

  Scenario: Preambles per AES3-3-2009
    Given DARS frame generation
    Then X preamble marks start of A subframe, start of 192-frame block
    And Y preamble marks start of B subframe
    And Z preamble marks start of A subframe, not start of block
    And timing reference point (TRP) is initial transition of X or Z preamble
    And preamble structure conforms to AES3-3 specifications

  Scenario: Integration with AES3-2009 repository
    Given external AES3-2009 implementation
    When DARS uses AES3 frame APIs
    Then AES3 frame creation succeeds
    And channel status encoding succeeds
    And preamble generation succeeds
    And no AES3 format is reimplemented in AES-11 code
```

**Dependencies**:
- External: AES3-2009 repository (https://github.com/zarfld/AES3-2009.git - **READY**)
- StR-FUNC-004 (External standards integration)
- StR-COMP-001 (AES-11 compliance depends on AES3)

**Risks**:
- **Risk**: AES3 repository API instability → **Mitigation**: Version pinning, upstream contributions
- **Risk**: Incomplete AES3 implementation → **Mitigation**: Validate AES3 repo completeness, contribute missing features

**Validation Method**: AES3 conformance testing, cross-validation with commercial AES3 equipment, interoperability testing

---

## StR-COMP-003: AES5-2018 Sampling Frequency Compliance

**ID**: StR-COMP-003  
**Priority**: High (P1)  
**Category**: Standards Integration  
**Source**: AES-11-2009 Section 2 (Normative References), Section 5.1.6  

**Stakeholder Statement**:
> "DARS must support all AES5 standard sampling frequencies to ensure compatibility across professional audio applications."

**Requirement**:
The solution shall support all sampling frequencies defined in AES5-2018 "AES recommended practice for professional digital audio applications employing pulse code modulation - Preferred Sampling Frequencies" per AES-11-2009 Section 5.1.6 normative requirement.

**Rationale**:
- AES-11-2009 Section 5.1.6: "Sampling frequencies distributed by a DARS should conform to AES5"
- AES5-2018 defines standard sampling frequencies for professional audio
- Support for multiple rates enables diverse application compatibility
- Rate relationships important for multi-rate synchronization (Annex D)

**AES5-2018 Standard Sampling Frequencies**:

**Basic Rates** (defined in AES5):
- 32 kHz (broadcast, telecommunications)
- 44.1 kHz (CD audio, consumer)
- 48 kHz (professional audio, video, broadcast)

**Double Rates**:
- 88.2 kHz (2 × 44.1 kHz)
- 96 kHz (2 × 48 kHz)

**Quadruple Rates**:
- 176.4 kHz (4 × 44.1 kHz)
- 192 kHz (4 × 48 kHz)

**Video-Referenced Rates** (AES-11 Table 1):
- 24 Hz, 24/1.001 Hz, 25 Hz, 30 Hz, 30/1.001 Hz video frame rates
- Sample counts per frame maintain integer or defined fractional relationships

**Success Criteria**:
- ✅ Support for 32, 44.1, 48, 96 kHz minimum (AES-11 Table 2 rates)
- ✅ Optional support for 88.2, 176.4, 192 kHz (extended rates)
- ✅ Rate relationships documented per AES5
- ✅ Video frame rate relationships per AES-11 Table 1
- ✅ Multi-rate synchronization per AES-11 Annex D
- ✅ Integration with external AES5-2018 repository validated

**Acceptance Criteria**:
```gherkin
Feature: AES5-2018 sampling frequency support
  As DARS implementation
  I want to support AES5 standard rates
  So that all professional applications are covered

  Scenario: Basic rates per AES5-2018
    Given DARS generator configuration
    When selecting sampling frequency
    Then 32 kHz is supported
    And 44.1 kHz is supported
    And 48 kHz is supported
    And all rates validated by AES5 repository API

  Scenario: Double and quadruple rates per AES5-2018
    Given DARS generator extended rate support
    Then 88.2 kHz (2 × 44.1) is supported
    And 96 kHz (2 × 48) is supported
    And 176.4 kHz (4 × 44.1) is optionally supported
    And 192 kHz (4 × 48) is optionally supported

  Scenario: Video-referenced rates per AES-11 Table 1
    Given video synchronization mode
    When video frame rate is 30 Hz at 48 kHz sampling
    Then 1920 samples per frame (integer ratio)
    And relationship documented in Table 1
    And synchronous locking achieved per Section 4.5.2

  Scenario: Multi-rate synchronization per AES-11 Annex D
    Given basic-rate DARS at 48 kHz
    When double-rate device locks at 96 kHz
    Then frequency relationship is exact 2:1 integer ratio
    And isochronous operation achieved per 4.2.1
    And phase relationship predictable

  Scenario: Integration with AES5-2018 repository
    Given external AES5-2018 implementation
    When DARS validates sampling frequency
    Then AES5 standard rate API is used
    And non-standard rates are rejected
    And rate relationships use AES5 definitions
    And no sampling rate constants duplicated in AES-11 code
```

**Dependencies**:
- External: AES5-2018 repository (https://github.com/zarfld/AES5-2018.git - **READY**)
- StR-FUNC-004 (External standards integration)
- StR-COMP-001 (AES-11 compliance references AES5)

**Risks**:
- **Risk**: AES5 repository incomplete rate definitions → **Mitigation**: Validate repository completeness, contribute updates
- **Risk**: Video rate relationships missing → **Mitigation**: Document in AES-11 implementation with AES-11 Table 1 references

**Validation Method**: Sample rate validation testing, multi-rate synchronization testing, video sync testing

---

## StR-COMP-004: Copyright and Intellectual Property Compliance

**ID**: StR-COMP-004  
**Priority**: Critical (P0)  
**Category**: Legal Compliance  
**Source**: Legal Requirements, Open Source Community  

**Stakeholder Statement**:
> "We need to ensure the implementation respects AES, SMPTE, and IEEE copyright while remaining usable as open source for commercial products."

**Requirement**:
The solution shall implement AES-11-2009 protocol functionality based on understanding of specifications without reproducing copyrighted content, using permissive open source licensing (MIT or Apache 2.0), and respecting all intellectual property rights of standards organizations.

**Rationale**:
- Standards documents (AES, SMPTE, IEEE) are copyrighted works
- Reproduction of specification text violates copyright
- Implementation of technical requirements is permitted
- Open source license enables commercial adoption
- Copyright compliance protects project and users from legal liability

**Copyright Compliance Requirements**:

**Prohibited Content Reproduction**:
- ❌ **DO NOT** copy specification text, tables, figures into source code comments
- ❌ **DO NOT** reproduce copyrighted diagrams or illustrations
- ❌ **DO NOT** paste specification sections into documentation
- ❌ **DO NOT** create derivative works reproducing substantial standard portions

**Permitted Implementation Practices**:
- ✅ **DO** reference standards by section number (e.g., "AES-11-2009, Section 5.1.3")
- ✅ **DO** implement protocol logic based on understanding of specifications
- ✅ **DO** use factual technical information not subject to copyright (constants, field sizes)
- ✅ **DO** create original code achieving compliance through specification understanding
- ✅ **DO** document implementation rationale with original explanations

**Success Criteria**:
- ✅ Zero copyrighted specification content reproduced in source code or documentation
- ✅ All standard references are citations by number/section only
- ✅ MIT or Apache 2.0 license applied to all original code
- ✅ Copyright notice credits AES, SMPTE, IEEE for specifications (without content reproduction)
- ✅ Legal review confirms copyright compliance
- ✅ README documents proper specification access from standards organizations

**Acceptance Criteria**:
```gherkin
Feature: Copyright-compliant implementation
  As open source maintainer
  I want to respect copyright
  So that project is legally sound

  Scenario: Source code copyright compliance
    Given all source code files
    When scanned for copyrighted content
    Then no AES specification text is present
    And no SMPTE specification text is present
    And no IEEE specification text is present
    And no copyrighted diagrams reproduced
    And all code is original implementation

  Scenario: Documentation copyright compliance
    Given all documentation files
    When reviewed for copyright violations
    Then specifications referenced by section number only
    And no specification text reproduced verbatim
    And explanations are original writing
    And implementation rationale documented independently

  Scenario: Copyright notices
    Given source code headers
    Then MIT or Apache 2.0 license header present
    And copyright notice for project maintainers present
    And disclaimer states implementation based on specification understanding
    And disclaimer states "no copyrighted content from standards reproduced"
    And references direct users to official standards for authoritative requirements

  Example copyright header:
    """
    /*
     * Copyright (c) 2025 AES11-2009 Project Contributors
     * SPDX-License-Identifier: MIT
     *
     * This file implements protocol functionality based on understanding of:
     * - AES-11-2009 (Digital Audio Reference Signals) - Copyright AES
     * - AES3-2009 (Digital Audio Interface) - Copyright AES
     * - AES5-2018 (Preferred Sampling Frequencies) - Copyright AES
     *
     * No copyrighted content from these specifications is reproduced.
     * Implementation is original work achieving compliance through
     * understanding of specification requirements.
     *
     * For authoritative requirements, refer to original specifications
     * available from Audio Engineering Society (www.aes.org/standards).
     */
    """

  Scenario: License file compliance
    Given LICENSE file in repository root
    Then full MIT or Apache 2.0 license text present
    And copyright year and holder specified
    And license applies to all original code
    And dependencies have compatible licenses documented
```

**Dependencies**:
- Legal review of licensing approach
- Standards organization guidance on implementation practices
- Open source license selection (MIT or Apache 2.0 recommended)

**Risks**:
- **Risk**: Unintentional copyright violation → **Mitigation**: Code review process, automated scanning
- **Risk**: License incompatibility with dependencies → **Mitigation**: License compatibility analysis

**Validation Method**: Legal review, copyright scanning tools, community review, standards organization consultation

---

## StR-COMP-005: Open Source Licensing for Commercial Use

**ID**: StR-COMP-005  
**Priority**: High (P1)  
**Category**: Licensing  
**Source**: Audio Equipment Manufacturers, Software Developers  

**Stakeholder Statement**:
> "We need a permissive open source license that allows commercial product integration without forcing us to open source our entire product codebase."

**Requirement**:
The solution shall use a permissive open source license (MIT or Apache 2.0) that permits commercial use, modification, and distribution without copyleft obligations, while requiring attribution and disclaimer of warranty.

**Rationale**:
- Commercial adoption critical for market success (StR-BUS-001)
- Copyleft licenses (GPL) prevent commercial integration
- Permissive licenses maximize ecosystem growth
- Attribution requirements ensure project credit and sustainability

**License Comparison**:

| License | Commercial Use | Modification | Distribution | Copyleft | Patent Grant |
|---------|----------------|--------------|--------------|----------|--------------|
| **MIT** | ✅ Yes | ✅ Yes | ✅ Yes | ❌ No | ❌ No |
| **Apache 2.0** | ✅ Yes | ✅ Yes | ✅ Yes | ❌ No | ✅ Yes |
| **GPL v3** | ✅ Yes | ✅ Yes | ✅ Yes | ✅ Yes | ✅ Yes |

**Recommended**: Apache 2.0 (includes patent grant protection)

**Success Criteria**:
- ✅ MIT or Apache 2.0 license applied to all original code
- ✅ LICENSE file in repository root with full license text
- ✅ Copyright header in every source file with SPDX identifier
- ✅ Dependency licenses documented and compatible
- ✅ Contribution guidelines include license agreement
- ✅ Legal opinion confirms commercial use without restrictions

**Acceptance Criteria**:
```gherkin
Feature: Permissive open source licensing
  As commercial product developer
  I want permissive license
  So that I can integrate without copyleft obligations

  Scenario: License selection
    Given project licensing decision
    Then Apache 2.0 license is recommended (patent protection)
    Or MIT license is acceptable (simpler, widely understood)
    And GPL family licenses are NOT used (copyleft conflict)
    And license documented in LICENSE file

  Scenario: License application
    Given all source code files
    Then every file has copyright header
    And SPDX license identifier present (SPDX-License-Identifier: Apache-2.0)
    And copyright year and holder specified
    And LICENSE file contains full Apache 2.0 text

  Scenario: Commercial use validation
    Given commercial product integration
    When product includes AES-11 library
    Then product NOT required to open source
    And product may use proprietary license
    And only attribution required (copyright notice, license reference)
    And no royalties or fees required

  Scenario: Dependency license compatibility
    Given external dependencies (AES3-2009, AES5-2018)
    When licenses analyzed
    Then all dependencies have compatible licenses
    And no GPL contamination
    And license compatibility documented
    And NOTICE file lists all dependency attributions
```

**Dependencies**:
- StR-COMP-004 (Copyright compliance prerequisite)
- External repository license compatibility
- Legal review of license selection

**Risks**:
- **Risk**: Dependency license incompatibility → **Mitigation**: Pre-integration license review
- **Risk**: Contributor license ambiguity → **Mitigation**: Contributor License Agreement (CLA) or Developer Certificate of Origin (DCO)

**Validation Method**: License scanning tools (REUSE compliance), legal review, Open Source Initiative (OSI) approval verification

---

**Summary**: Compliance requirements ensure 100% AES-11-2009 standards conformance, proper integration with normative AES3 and AES5 standards, copyright and intellectual property protection, and permissive open source licensing enabling commercial adoption while respecting all legal obligations.
