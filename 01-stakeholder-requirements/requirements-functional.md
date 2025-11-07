# Stakeholder Requirements - Functional Requirements

**Part of**: Stakeholder Requirements Specification (StRS-AES11-001)  
**Section**: 3.3 Functional Requirements (Core Capabilities)

---

## StR-FUNC-001: Core DARS Protocol Implementation

**ID**: StR-FUNC-001  
**Priority**: Critical (P0)  
**Category**: Protocol Implementation  
**Source**: All Stakeholders, AES-11-2009 Standard Sections 4-5  

**Stakeholder Statement**:
> "We need a complete, standards-compliant implementation of all four AES-11 synchronization methods (DARS-referenced, audio-input-referenced, video-referenced, GPS-referenced) supporting both Grade 1 and Grade 2 accuracy levels."

**Requirement**:
The solution shall implement the complete AES-11-2009 Digital Audio Reference Signal (DARS) protocol including all four synchronization methods, Grade 1 and Grade 2 accuracy levels, channel status encoding, and timing reference point management per AES-11-2009 Sections 4 and 5.

**Rationale**:
- AES-11-2009 defines four synchronization methods for different use cases
- Grade 1 (±1 ppm) required for multi-studio complexes
- Grade 2 (±10 ppm) sufficient for single-studio applications
- Complete implementation ensures universal applicability
- Standards compliance mandatory for certification

**Success Criteria**:
- ✅ All four synchronization methods implemented per AES-11 Section 4.2
- ✅ Grade 1 and Grade 2 DARS generation and locking per Section 5.1.2
- ✅ Channel status encoding per AES-11 Section 5.1.3 (byte 4, bits 0-1)
- ✅ Timing reference point (TRP) detection and generation per Section 3.3
- ✅ Support for all AES5 sampling frequencies (32, 44.1, 48, 96 kHz) per Section 5.1.6
- ✅ Video reference alignment per Section 5.3.4 (half-amplitude point of sync pulse)
- ✅ Date and time encoding in channel status per Section 5.1.5

**Acceptance Criteria**:

```gherkin
Feature: DARS-referenced synchronization (Section 4.2.1)
  As an audio equipment manufacturer
  I want to synchronize to distributed DARS
  So that all equipment locks to same reference

  Scenario: Grade 1 DARS generation
    Given system is configured for Grade 1 operation
    And high-precision reference clock is available
    When DARS generator is initialized at 48 kHz
    Then DARS signal conforms to AES3 format
    And channel status byte 4 bits 0-1 = 01 (Grade 1 identifier)
    And timing reference point is X/Z preamble transition
    And frequency accuracy is within ±1 ppm
    And DARS is marked as "not linear PCM" in channel status

  Scenario: Grade 2 DARS generation
    Given system is configured for Grade 2 operation
    When DARS generator is initialized at 48 kHz
    Then channel status byte 4 bits 0-1 = 10 (Grade 2 identifier)
    And frequency accuracy is within ±10 ppm
    And capture range supports ±50 ppm input signals

  Scenario: DARS locking
    Given external Grade 1 DARS signal at 48 kHz
    When equipment locks to DARS reference
    Then internal sample clock locks within ±1 ppm
    And phase offset is within ±5% of AES3 frame period (±1.0 µs at 48 kHz)
    And lock status is maintained continuously
    And lock-loss detection occurs within 100 ms

Feature: Audio input-referenced synchronization (Section 4.2.2)
  As a system integrator
  I want to lock to embedded clock in audio input
  So that I can synchronize without separate DARS

  Scenario: Audio input locking
    Given digital audio input signal at 48 kHz
    When equipment configured for audio-input-referenced mode
    Then internal sample clock locks to input signal clock
    And phase relationship is maintained within ±25% of frame period (±5.2 µs)
    And device delay remains constant and known
    And sufficient hysteresis prevents sample slips during jitter

Feature: Video-referenced synchronization (Section 4.2.3, 4.5)
  As a broadcast engineer
  I want to synchronize audio to video reference
  So that audio and video remain locked

  Scenario: Integer ratio video sync (48 kHz @ 30 Hz)
    Given video reference at 30 Hz (1920 samples per frame)
    When DARS locked to video reference
    Then X/Z preamble aligns to half-amplitude point of line 1 sync pulse
    And phase tolerance is ±5% of AES3 frame (±1.0 µs at 48 kHz)
    And audio-video relationship per AES-11 Table 1 is maintained
    And synchronous locking (not just isochronous) is achieved

  Scenario: Non-integer ratio video sync (48 kHz @ 29.97 Hz NTSC)
    Given video reference at 30/1.001 Hz (1601.6 samples per frame)
    When DARS locked to video reference
    Then alignment occurs on every 5th frame (for NTSC)
    And ±1 sample offset tolerance is accepted
    And hysteresis prevents random sample slips
    And video frame indicator is required for predictable phasing

Feature: GPS-referenced synchronization (Section 4.2.4)
  As a multi-site facility operator
  I want GPS-referenced synchronization
  So that geographically distributed sites are locked

  Scenario: GPS reference locking
    Given GPS receiver provides 1 PPS (pulse per second) signal
    When DARS generator locks to GPS reference
    Then frequency accuracy matches GPS (better than ±1 ppm)
    And phase locked to GPS 1 PPS signal
    And time-of-day encoded in channel status bytes 18-21
    And sample address code synchronized to UTC time
```

**Dependencies**:
- External: AES3-2009 (frame format, channel status) - https://github.com/zarfld/AES3-2009.git
- External: AES5-2018 (sampling frequencies) - https://github.com/zarfld/AES5-2018.git
- StR-FUNC-002 (Hardware Abstraction Layer for platform I/O)
- StR-PERF-001 (Timing precision requirements)

**Risks**:
- **Risk**: Real-time performance insufficient for embedded systems → **Mitigation**: ARM Cortex-M7 profiling, optimization
- **Risk**: AES3/AES5 integration complexity → **Mitigation**: External repositories provide tested implementations

**Validation Method**: Conformance test suite, AES Technical Council review, multi-platform testing

---

## StR-FUNC-002: Hardware Abstraction Layer (HAL)

**ID**: StR-FUNC-002  
**Priority**: Critical (P0)  
**Category**: Platform Independence  
**Source**: Audio Equipment Manufacturers, Embedded Developers  

**Stakeholder Statement**:
> "We need a clean hardware abstraction layer that lets us port to any platform by implementing a small set of platform-specific functions, without touching the core DARS protocol code."

**Requirement**:
The solution shall provide a hardware abstraction layer (HAL) with well-defined interfaces for audio I/O, timing, and synchronization primitives that enable platform porting through HAL implementation without modifying core protocol code.

**Rationale**:
- Platform independence is critical business requirement (StR-BUS-002)
- Hardware varies widely (ASIO, ALSA, CoreAudio, custom embedded)
- Core protocol code must be hardware-agnostic for maintainability
- HAL design enables new platforms without core modifications

**Success Criteria**:
- ✅ Zero vendor or OS-specific code in core implementation
- ✅ HAL interface design supports ARM Cortex-M7 and x86-64 platforms
- ✅ New platform addition requires <80 hours engineering effort
- ✅ HAL validation tool verifies implementation correctness
- ✅ Performance overhead <5% compared to platform-specific implementation

**HAL Interface Requirements**:

**Audio I/O Interface**:
```c
typedef struct {
    // Send AES3 frame to hardware output
    int (*send_audio_frame)(const aes3_frame_t* frame);
    
    // Receive AES3 frame from hardware input
    int (*receive_audio_frame)(aes3_frame_t* frame);
    
    // Set audio sample rate (32, 44.1, 48, 96 kHz)
    int (*set_sample_rate)(uint32_t sample_rate_hz);
    
    // Get current hardware sample rate
    uint32_t (*get_sample_rate)(void);
    
} audio_hal_t;
```

**Timing Interface**:
```c
typedef struct {
    // Get high-resolution timestamp in nanoseconds
    uint64_t (*get_time_ns)(void);
    
    // Get sample-accurate timestamp (sample count since start)
    uint64_t (*get_sample_time)(void);
    
    // Sleep/delay for specified microseconds (non-real-time only)
    void (*delay_us)(uint32_t microseconds);
    
} timing_hal_t;
```

**Synchronization Interface**:
```c
typedef struct {
    // Get timing reference from external source (video, GPS, etc.)
    int (*get_timing_reference)(timing_reference_t* ref);
    
    // Lock to external reference frequency
    int (*lock_to_reference)(uint32_t reference_freq_hz);
    
    // Get lock status and frequency error
    int (*get_lock_status)(lock_status_t* status);
    
} sync_hal_t;
```

**Acceptance Criteria**:

```gherkin
Feature: Platform-agnostic HAL design
  As an embedded developer
  I want to implement platform-specific HAL
  So that I can use library on my custom hardware

  Scenario: HAL implementation isolation
    Given core DARS protocol code
    When developer inspects source files
    Then no vendor-specific includes are present
    And no OS-specific system calls are present
    And all hardware access goes through HAL interfaces
    And HAL interfaces use C function pointers for portability

  Scenario: New platform porting
    Given new target platform (e.g., RISC-V embedded)
    When developer implements HAL interfaces
    Then audio_hal_t interface provides frame I/O
    And timing_hal_t interface provides nanosecond timestamps
    And sync_hal_t interface provides reference locking
    And implementation is <2000 lines of code
    And core protocol code requires zero modifications
    And porting effort is <80 hours

  Scenario: HAL validation
    Given developer has implemented platform HAL
    When they run HAL validation tool
    Then tool tests all HAL interface functions
    And validates timing accuracy (nanosecond precision)
    And validates audio I/O correctness (frame format, alignment)
    And generates pass/fail report
    And identifies performance bottlenecks
```

**Dependencies**:
- StR-FUNC-001 (Core DARS Protocol needs HAL for hardware access)
- StR-USER-004 (Diagnostic tools for HAL validation)

**Risks**:
- **Risk**: HAL abstraction overhead affects real-time performance → **Mitigation**: Zero-cost abstractions, inline functions, profiling
- **Risk**: HAL interface insufficient for some platforms → **Mitigation**: Extensible design, version 1.x refinements

**Validation Method**: Multi-platform implementation, performance profiling, HAL validation tool

---

## StR-FUNC-003: Conformance Testing Suite

**ID**: StR-FUNC-003  
**Priority**: High (P1)  
**Category**: Quality Assurance  
**Source**: QA Engineers, Audio Equipment Manufacturers, AES Standards Body  

**Stakeholder Statement**:
> "We need an automated conformance test suite that validates all AES-11-2009 requirements so we can certify products without manual testing or expensive external certification labs."

**Requirement**:
The solution shall provide a comprehensive automated conformance test suite that validates all AES-11-2009 requirements including timing precision, capture range, phase relationships, channel status encoding, and video synchronization relationships.

**Rationale**:
- Certification readiness is critical business requirement (StR-BUS-001)
- Manual conformance testing is expensive and error-prone
- Automated testing enables continuous validation during development
- Conformance reports required for AES certification process

**Success Criteria**:
- ✅ Test coverage for all AES-11-2009 Section 5 requirements
- ✅ Test coverage for all AES-11-2009 Section 6 requirements
- ✅ Grade 1 and Grade 2 accuracy validation
- ✅ Capture range testing per Section 5.2.2
- ✅ Timing relationship validation per Section 5.3
- ✅ Video reference alignment validation per Section 5.3.4
- ✅ Automated test execution with pass/fail reporting
- ✅ Certification-ready conformance report generation

**Acceptance Criteria**:

```gherkin
Feature: AES-11 Section 5 conformance testing
  As a QA engineer
  I want automated conformance tests
  So that I can validate AES-11 compliance

  Scenario: Grade 1 frequency accuracy (Section 5.2.1.1)
    Given DARS generator configured for Grade 1
    When test measures frequency over 1 minute
    Then long-term accuracy is within ±1 ppm
    And short-term stability is validated
    And test result is PASS/FAIL with measured value

  Scenario: Grade 2 frequency accuracy (Section 5.2.1.2)
    Given DARS generator configured for Grade 2
    When test measures frequency over 1 minute
    Then accuracy is within ±10 ppm
    And result is documented in conformance report

  Scenario: Grade 1 capture range (Section 5.2.2.1)
    Given equipment configured for Grade 1 operation
    When external reference swept from -2 ppm to +2 ppm
    Then equipment locks at all frequencies within range
    And lock is maintained continuously
    And lock-loss occurs outside ±2 ppm range

  Scenario: Grade 2 capture range (Section 5.2.2.2)
    Given equipment configured for Grade 2 operation
    When external reference swept from -50 ppm to +50 ppm
    Then equipment locks at all frequencies within range

  Scenario: Output timing phase (Section 5.3.1.1)
    Given DARS reference signal at 48 kHz
    And equipment output synchronized to DARS
    When test measures phase difference at connector points
    Then phase offset is within ±5% of AES3 frame period (±1.0 µs)
    And measurement is repeated 1000 times for statistical validation
    And all samples meet requirements

  Scenario: Input timing phase tolerance (Section 5.3.1.2)
    Given external input with phase offset from DARS
    When phase offset varied from -25% to +25% of frame period
    Then device delay remains constant and known
    And no sample slips occur
    And sufficient hysteresis is demonstrated

Feature: AES-11 Section 6 clock specifications
  As a QA engineer
  I want to validate sampling clock quality
  So that ADC/DAC converters achieve best performance

  Scenario: Jitter performance validation
    Given DARS-locked sampling clock
    When jitter measured per AES3-4-2009 specifications
    Then random jitter meets AES3 requirements
    And jitter modulation is within limits
    And timing precision exceeds Grade 1/2 requirements

Feature: Conformance report generation
  As an audio equipment manufacturer
  I want certification-ready conformance report
  So that I can submit for AES certification

  Scenario: Automated report generation
    Given all conformance tests completed
    When report generation requested
    Then report includes all Section 5 test results
    And report includes all Section 6 test results
    And each requirement has PASS/FAIL status
    And measured values and tolerances are documented
    And test conditions and equipment are documented
    And report is formatted per AES submission requirements
```

**Dependencies**:
- StR-FUNC-001 (Core DARS Protocol to be tested)
- StR-PERF-001 through StR-PERF-005 (Performance requirements to validate)
- StR-USER-004 (Diagnostic tools for measurements)

**Risks**:
- **Risk**: Test equipment unavailable for validation → **Mitigation**: Software-based stimulus/measurement, hardware test lab partnerships
- **Risk**: Test coverage gaps → **Mitigation**: AES Technical Council review of test suite

**Validation Method**: Test suite execution on multiple platforms, AES Technical Council review, comparison with commercial test equipment

---

## StR-FUNC-004: External AES Standards Integration

**ID**: StR-FUNC-004  
**Priority**: Critical (P0)  
**Category**: Standards Integration  
**Source**: Technical Requirements, AES-11 Dependencies  

**Stakeholder Statement**:
> "AES-11 builds upon AES3 (digital audio interface) and AES5 (sampling frequencies). We need seamless integration with these standards without reimplementing them or creating dependencies on their internal details."

**Requirement**:
The solution shall integrate with external AES3-2009 and AES5-2018 implementations via well-defined interfaces, using these standards' repositories as dependencies without reimplementing frame formats, channel status, or sampling frequency definitions.

**Rationale**:
- AES-11 DARS is transported in AES3 frames (Section 5.1.1)
- AES-11 references AES5 sampling frequencies (Section 5.1.6)
- Reusing existing implementations prevents duplication and ensures consistency
- External repositories enable independent updates and version management

**Success Criteria**:
- ✅ AES3-2009 repository integrated for frame format and channel status
- ✅ AES5-2018 repository integrated for sampling frequency definitions
- ✅ Clean interface boundaries prevent tight coupling
- ✅ Version pinning prevents breaking changes
- ✅ Integration tests validate cross-standard compatibility

**External Repository Dependencies**:

**AES3-2009 Integration** (https://github.com/zarfld/AES3-2009.git - **READY**):
```c
// Use AES3 frame structures
#include <AES3/frames/aes3_frame.h>
#include <AES3/preambles/preamble_detector.h>
#include <AES3/channel/channel_status.h>

// DARS uses AES3 frame format
aes3_frame_t* dars_frame = aes3_create_frame();
aes3_set_preamble(dars_frame, AES3_PREAMBLE_X); // Timing reference point
aes3_set_channel_status_byte(dars_frame, 4, dars_grade_bits); // Grade 1/2
```

**AES5-2018 Integration** (https://github.com/zarfld/AES5-2018.git - **READY**):
```c
// Use AES5 sampling rate definitions
#include <AES5/rates/standard_sampling_rates.h>

// DARS supports all AES5 rates
uint32_t sample_rate = aes5_get_standard_rate(AES5_RATE_48KHZ);
bool is_valid = aes5_validate_sampling_frequency(sample_rate);
```

**Acceptance Criteria**:

```gherkin
Feature: AES3-2009 integration
  As a DARS protocol implementer
  I want to use AES3 frame format
  So that DARS conforms to AES3 standard

  Scenario: DARS frame generation using AES3
    Given DARS generator initialized at 48 kHz
    When generating DARS output
    Then AES3 frame structure is used
    And X/Z preambles mark timing reference points
    And channel status encoding uses AES3 format
    And frame alignment follows AES3 specifications
    And no AES3 format is reimplemented in AES-11 code

  Scenario: Channel status encoding
    Given DARS Grade 1 configuration
    When encoding channel status
    Then AES3 channel status API is used
    And byte 4 bits 0-1 set to 01 (Grade 1) per AES-11
    And "not linear PCM" flag set per AES-11 Section 5.1.4
    And date/time metadata encoded per AES-11 Section 5.1.5

Feature: AES5-2018 integration
  As a DARS protocol implementer
  I want to use AES5 sampling frequencies
  So that DARS supports all standard rates

  Scenario: Sampling rate selection
    Given application requests DARS at specific rate
    When validating sample rate
    Then AES5 standard rates API is used
    And rates 32, 44.1, 48, 96, 192 kHz are supported
    And non-standard rates are rejected with clear error
    And no sampling rate definitions are duplicated

  Scenario: Multi-rate synchronization (Annex D)
    Given base DARS at 48 kHz
    When locking double-rate 96 kHz device
    Then AES5 rate relationships are used
    And integer frequency ratios are maintained
    And isochronous operation is achieved per AES-11 Section 4.2.1

Feature: Dependency management
  As a build system maintainer
  I want clean dependency management
  So that updates don't break builds

  Scenario: CMake FetchContent integration
    Given AES11-2009 CMake configuration
    When building project
    Then AES3-2009 fetched from GitHub repository
    And AES5-2018 fetched from GitHub repository
    And specific version tags are pinned
    And header include paths are properly configured
    And linking succeeds without errors

  Scenario: Version compatibility
    Given AES3-2009 v2.x and AES5-2018 v1.x
    When AES-11 implementation uses interfaces
    Then APIs are stable across minor versions
    And breaking changes are detected at compile time
    And integration tests validate compatibility
```

**Dependencies**:
- External: AES3-2009 repository (frame format, preambles, channel status) - **READY**
- External: AES5-2018 repository (sampling frequencies) - **READY**
- CMake FetchContent or Git submodules for dependency management

**Risks**:
- **Risk**: External repositories become unmaintained → **Mitigation**: Fork capabilities, contribute upstream
- **Risk**: Breaking changes in dependencies → **Mitigation**: Version pinning, integration tests in CI

**Validation Method**: Integration testing, cross-repository compatibility testing, dependency resolution verification

---

**Summary**: Functional requirements define the core DARS protocol implementation, platform-agnostic HAL, comprehensive conformance testing, and integration with external AES3/AES5 standards that enable complete AES-11-2009 compliance.
