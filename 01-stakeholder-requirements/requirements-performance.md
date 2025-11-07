# Stakeholder Requirements - Performance Requirements

**Part of**: Stakeholder Requirements Specification (StRS-AES11-001)  
**Section**: 3.4 Performance Requirements (Non-Functional Quality Attributes)

---

## StR-PERF-001: Timing Precision and Frequency Accuracy

**ID**: StR-PERF-001  
**Priority**: Critical (P0)  
**Category**: Real-Time Performance  
**Source**: AES-11-2009 Sections 5.2, 6.1  

**Stakeholder Statement**:
> "We need timing precision that meets AES-11 Grade 1 (±1 ppm) and Grade 2 (±10 ppm) specifications across all supported platforms, verified by conformance testing."

**Requirement**:
The solution shall maintain long-term frequency accuracy within ±1 ppm for Grade 1 and ±10 ppm for Grade 2 operation per AES-11-2009 Section 5.2, with enhanced timing accuracy for ADC/DAC conversion clocks per Section 6.1.

**Rationale (from AES-11-2009)**:
- **Section 5.2.1.1**: Grade 1 DARS signals intended for multi-studio complex synchronization
- **Section 5.2.1.2**: Grade 2 DARS for single-studio applications
- **Section 6.1**: Converter clocks require higher precision than DARS distribution
- Professional audio requires sample-accurate synchronization for digital processing

**Numerical Requirements (from AES-11-2009)**:

| Grade | Frequency Tolerance | Application | AES-11 Reference |
|-------|---------------------|-------------|------------------|
| **Grade 1** | ±1 ppm (±0.0001%) | Multi-studio complex | Section 5.2.1.1 |
| **Grade 2** | ±10 ppm (±0.001%) | Single studio | Section 5.2.1.2 |
| **ADC/DAC Clocks** | Better than Grade 1 | Analog conversion | Section 6.1 |

**Example Calculations**:
```
48 kHz ± 1 ppm = 48,000 Hz ± 0.048 Hz = 47,999.952 to 48,000.048 Hz
48 kHz ± 10 ppm = 48,000 Hz ± 0.48 Hz = 47,999.52 to 48,000.48 Hz

Over 1 hour at 48 kHz:
±1 ppm = ±172.8 samples drift maximum
±10 ppm = ±1728 samples drift maximum
```

**Success Criteria**:
- ✅ Grade 1: Long-term frequency within ±1 ppm (measured over ≥1 minute)
- ✅ Grade 2: Long-term frequency within ±10 ppm (measured over ≥1 minute)
- ✅ ADC/DAC clock jitter performance per AES3-4-2009
- ✅ Measurement accuracy ±0.1 ppm for validation
- ✅ Temperature stability documented (operating range: 0°C to 45°C)

**Acceptance Criteria**:
```gherkin
Feature: Grade 1 frequency accuracy (AES-11 Section 5.2.1.1)
  As a multi-studio facility
  I want ±1 ppm frequency accuracy
  So that all studios remain synchronized

  Scenario: Grade 1 long-term accuracy at 48 kHz
    Given DARS generator in Grade 1 mode
    And nominal frequency is 48,000 Hz
    When frequency measured over 60 seconds
    Then average frequency is 48,000 Hz ±0.048 Hz (±1 ppm)
    And maximum deviation over measurement is ≤±0.048 Hz
    And result documented in conformance report

  Scenario: Grade 1 temperature stability
    Given DARS generator operating at 25°C
    When temperature varied from 0°C to 45°C
    Then frequency remains within ±1 ppm across range
    And temperature coefficient documented

Feature: Grade 2 frequency accuracy (AES-11 Section 5.2.1.2)
  As a single-studio facility
  I want ±10 ppm frequency accuracy
  So that equipment remains synchronized

  Scenario: Grade 2 long-term accuracy at 48 kHz
    Given DARS generator in Grade 2 mode
    When frequency measured over 60 seconds
    Then average frequency is 48,000 Hz ±0.48 Hz (±10 ppm)

Feature: ADC/DAC clock quality (AES-11 Section 6.1)
  As an audio converter
  I want enhanced clock precision
  So that conversion quality is maximized

  Scenario: Converter clock jitter
    Given DARS-locked sampling clock for ADC/DAC
    When jitter measured per AES3-4-2009
    Then random jitter is better than Grade 1 requirements
    And jitter modulation is within AES3 limits
    And timing precision exceeds DARS distribution requirements
```

**Dependencies**:
- StR-FUNC-001 (Core DARS protocol must implement frequency control)
- HAL timing interface (nanosecond-precision timestamps)
- Reference oscillator quality (TCXO or better for Grade 1)

**Risks**:
- **Risk**: Platform clock instability → **Mitigation**: TCXO/OCXO requirements documented, calibration procedures
- **Risk**: Temperature drift → **Mitigation**: Temperature compensation, stable environment requirements

**Validation Method**: Frequency counter measurements (±0.1 ppm accuracy), conformance test suite, temperature chamber testing

---

## StR-PERF-002: Capture Range and Locking Performance

**ID**: StR-PERF-002  
**Priority**: High (P1)  
**Category**: Synchronization Performance  
**Source**: AES-11-2009 Section 5.2.2  

**Stakeholder Statement**:
> "We need equipment to lock reliably to external references within specified capture ranges, even when initial frequency offset is large or reference signal quality is poor."

**Requirement**:
The solution shall lock to external references within ±2 ppm for Grade 1 and ±50 ppm for Grade 2 equipment per AES-11-2009 Section 5.2.2, with acquisition time <2 seconds and stable lock maintained under jitter conditions.

**Rationale (from AES-11-2009)**:
- **Section 5.2.2.1**: Grade 1 equipment locks to Grade 1 references (±2 ppm range)
- **Section 5.2.2.2**: Grade 2 equipment locks to wider range (±50 ppm)
- Capture range must exceed frequency tolerance to ensure reliable locking
- Fast acquisition time minimizes startup delay

**Numerical Requirements (from AES-11-2009)**:

| Grade | Capture Range | Lock Acquisition Time | Hysteresis |
|-------|---------------|----------------------|------------|
| **Grade 1** | ±2 ppm | <2 seconds | Sufficient to cover jitter |
| **Grade 2** | ±50 ppm | <2 seconds | Per AES3 jitter tolerance |

**Example Capture Ranges**:
```
48 kHz Grade 1: 48,000 Hz ± 0.096 Hz = 47,999.904 to 48,000.096 Hz
48 kHz Grade 2: 48,000 Hz ± 2.4 Hz = 47,997.6 to 48,002.4 Hz
```

**Success Criteria**:
- ✅ Grade 1: Locks to references within ±2 ppm of nominal
- ✅ Grade 2: Locks to references within ±50 ppm of nominal
- ✅ Lock acquisition time <2 seconds (measured from signal present to lock indicator)
- ✅ Lock maintained with input jitter per AES3-4-2009 specifications
- ✅ Hysteresis prevents sample slips during normal jitter

**Acceptance Criteria**:
```gherkin
Feature: Grade 1 capture range (AES-11 Section 5.2.2.1)
  As Grade 1 equipment
  I want to lock within ±2 ppm range
  So that I synchronize to Grade 1 DARS

  Scenario: Grade 1 capture range at 48 kHz
    Given Grade 1 equipment free-running at 48,000 Hz
    When external Grade 1 DARS at 47,999.904 Hz applied (-2 ppm)
    Then equipment locks within 2 seconds
    And lock indicator activates
    And frequency error reduces to <±0.1 ppm
    When external DARS changed to 48,000.096 Hz (+2 ppm)
    Then equipment maintains or reacquires lock within 2 seconds

  Scenario: Grade 1 lock-loss boundary
    Given equipment locked to 48,000 Hz reference
    When reference frequency changed to 48,000.120 Hz (+2.5 ppm, outside range)
    Then equipment loses lock
    And lock-loss indicator activates
    And application receives callback notification

Feature: Grade 2 capture range (AES-11 Section 5.2.2.2)
  As Grade 2 equipment
  I want to lock within ±50 ppm range
  So that I synchronize to wider frequency variations

  Scenario: Grade 2 capture range at 48 kHz
    Given Grade 2 equipment free-running at 48,000 Hz
    When external reference at 47,997.6 Hz applied (-50 ppm)
    Then equipment locks within 2 seconds
    When reference changed to 48,002.4 Hz (+50 ppm)
    Then equipment maintains or reacquires lock within 2 seconds

Feature: Lock stability and hysteresis
  As equipment maintaining lock
  I want hysteresis to prevent sample slips
  So that jitter doesn't cause lock instability

  Scenario: Jitter tolerance
    Given equipment locked to 48 kHz reference
    When reference has jitter per AES3-4-2009 limits
    Then lock is maintained continuously
    And no sample slips occur
    And device delay remains constant
    And hysteresis sufficient per AES-11 Section 5.3.2
```

**Dependencies**:
- StR-PERF-001 (Frequency accuracy after lock achieved)
- StR-PERF-004 (Jitter tolerance specifications)
- Phase-locked loop (PLL) design and tuning

**Risks**:
- **Risk**: PLL instability during acquisition → **Mitigation**: Conservative loop filter design, simulation
- **Risk**: False lock detection → **Mitigation**: Multi-stage lock validation

**Validation Method**: Frequency sweep testing, lock time measurements, jitter injection testing

---

## StR-PERF-003: Phase Relationship and Timing Tolerance

**ID**: StR-PERF-003  
**Priority**: Critical (P0)  
**Category**: Synchronization Precision  
**Source**: AES-11-2009 Section 5.3  

**Stakeholder Statement**:
> "We need precise phase relationships between DARS and audio signals so that synchronous digital processing can occur without sample rate conversion or buffering."

**Requirement**:
The solution shall maintain phase relationships per AES-11-2009 Section 5.3, with output timing within ±5% of AES3 frame period (±18°) and input tolerance of ±25% of frame period (±90°) while maintaining constant device delay.

**Rationale (from AES-11-2009)**:
- **Section 5.3.1.1**: Output timing phase within ±5% enables synchronous operation
- **Section 5.3.1.2**: Input tolerance ±25% with constant delay prevents sample slips
- **Section 5.3.4**: Video reference alignment to half-amplitude point ±5% tolerance
- Precise phase relationships eliminate need for sample rate conversion

**Numerical Requirements (from AES-11-2009 Table 2)**:

| Sample Rate | Frame Period (1/fs) | Output Tolerance (±5%) | Input Tolerance (±25%) |
|-------------|---------------------|------------------------|------------------------|
| **32 kHz** | 31.25 µs | ±1.6 µs | ±7.8 µs |
| **44.1 kHz** | 22.68 µs | ±1.1 µs | ±5.7 µs |
| **48 kHz** | 20.83 µs | ±1.0 µs | ±5.2 µs |
| **96 kHz** | 10.41 µs | ±0.5 µs | ±2.6 µs |

**Timing Reference Point (TRP)** (Section 3.3):
- Initial transition of X or Z preamble of AES3 frame
- Defines phase relationship between DARS and audio signals

**Success Criteria**:
- ✅ Output phase within ±5% of frame period (per Table 2)
- ✅ Input tolerance ±25% with constant device delay
- ✅ Video reference alignment within ±5% per Section 5.3.4
- ✅ Overall system phase tolerance ±10% (±5% DARS + ±5% outputs)
- ✅ Device delay constant and documented

**Acceptance Criteria**:
```gherkin
Feature: Output timing phase (AES-11 Section 5.3.1.1, Table 2)
  As synchronized equipment
  I want output phase within ±5% of DARS
  So that outputs are synchronous

  Scenario: Output phase at 48 kHz
    Given DARS reference at 48 kHz
    And equipment synchronized to DARS
    When output phase measured at connector points
    Then phase difference is within ±1.0 µs (±5% of 20.83 µs frame)
    And timing reference point is X/Z preamble transition
    And measurement repeated 1000 times shows 100% compliance

  Scenario: Output phase at 96 kHz (double-rate)
    Given DARS reference at 48 kHz (basic rate)
    And equipment output at 96 kHz (double-rate)
    When output phase measured
    Then phase difference within ±0.5 µs (±5% of 10.41 µs frame)
    And timing reference at 96 kHz frame rate

Feature: Input timing tolerance (AES-11 Section 5.3.1.2, Table 2)
  As receiving equipment
  I want to accept inputs within ±25% phase
  So that device delay remains constant without sample slips

  Scenario: Input phase tolerance at 48 kHz
    Given equipment synchronized to DARS at 48 kHz
    When input signal phase offset from -5.2 µs to +5.2 µs
    Then device delay remains constant and known
    And no sample slips occur
    And sufficient hysteresis prevents jitter-induced slips

  Scenario: Random phase inputs (Section 5.3.2)
    Given input signals with random fixed phase
    When phase outside ±25% tolerance
    Then receiver accepts inputs with sufficient hysteresis
    And sample slips avoided during jitter
    And phase specific for slip occurrence is documented

Feature: Video reference alignment (AES-11 Section 5.3.4)
  As video-synchronized system
  I want DARS aligned to video sync pulse
  So that audio and video remain locked

  Scenario: Video reference timing at 48 kHz @ 30 Hz
    Given video reference at 30 Hz (1920 samples per frame)
    When DARS aligned to video
    Then X/Z preamble aligns to half-amplitude point of line 1 sync
    And phase tolerance is ±1.0 µs (±5% of frame period)
    And alignment verified on every video frame (integer ratio)

  Scenario: Overall phase tolerance (Section 5.3.5)
    Given video reference system
    Then DARS to video: ±5% tolerance
    And DARS to audio outputs: ±5% tolerance
    And total system tolerance: ±10% of AES3 frame period
```

**Dependencies**:
- StR-PERF-001 (Frequency accuracy enables phase lock)
- StR-FUNC-001 (TRP detection and generation)
- High-resolution timing measurements (nanosecond accuracy)

**Risks**:
- **Risk**: Cable delays affect phase → **Mitigation**: Document cable length limits, calibration procedures
- **Risk**: Equipment instrumental delays vary → **Mitigation**: Characterization, documentation

**Validation Method**: Oscilloscope measurements, TRP detection verification, phase analyzer tools

---

## StR-PERF-004: Jitter Performance

**ID**: StR-PERF-004  
**Priority**: High (P1)  
**Category**: Signal Quality  
**Source**: AES-11-2009 Section 6.1, AES3-4-2009  

**Stakeholder Statement**:
> "We need low-jitter clocks for ADC/DAC converters to achieve maximum analog conversion quality, beyond what's required for DARS distribution."

**Requirement**:
The solution shall provide sampling clocks with enhanced timing accuracy and reduced jitter compared to DARS distribution requirements, meeting AES3-4-2009 jitter specifications for optimal ADC/DAC converter performance.

**Rationale (from AES-11-2009)**:
- **Section 6.1**: ADC/DAC clocks need increased timing accuracy beyond DARS
- **Section 5.4.2.2**: Jitter noise causes timing error accumulation
- Random jitter and jitter modulation degrade analog conversion quality
- AES3-4-2009 defines jitter tolerance requirements for interfaces

**Jitter Specifications (referenced from AES3-4-2009)**:

| Jitter Type | Requirement | Impact |
|-------------|-------------|--------|
| **Random Jitter** | <10 ns RMS | Affects SNR of converters |
| **Jitter Modulation** | Per AES3-4-2009 limits | Accumulates as timing error |
| **Peak-to-Peak** | <100 ns (typical) | Prevents interface errors |

**Success Criteria**:
- ✅ Random jitter <10 ns RMS for converter clocks
- ✅ Jitter modulation within AES3-4-2009 specifications
- ✅ DARS-locked clocks have better jitter than free-running Grade 1/2
- ✅ Jitter measurement and reporting via diagnostic APIs
- ✅ Conformance to AES3-4-2009 receiver jitter tolerance

**Acceptance Criteria**:
```gherkin
Feature: ADC/DAC clock jitter quality (AES-11 Section 6.1)
  As an analog converter
  I want low-jitter sampling clock
  So that conversion quality is maximized

  Scenario: Random jitter performance
    Given DARS-locked sampling clock at 48 kHz
    When jitter measured with time interval analyzer
    Then RMS jitter is <10 ns
    And peak-to-peak jitter <100 ns
    And jitter performance documented in datasheet

  Scenario: Jitter vs free-running clock
    Given Grade 1 DARS-locked clock
    And free-running Grade 1 oscillator
    When jitter compared
    Then DARS-locked clock has better jitter precision
    And timing accuracy exceeds Grade 1 frequency requirement

Feature: Jitter modulation (AES-11 Section 5.4.2.2, AES3-4)
  As digital audio interface
  I want jitter modulation within limits
  So that timing errors don't accumulate

  Scenario: Low-frequency jitter modulation
    Given DARS distribution over long cable
    When jitter modulation measured <1 kHz
    Then amplitude and frequency meet AES3-4-2009 limits
    And timing error accumulation is bounded
    And conformance report documents measurements

Feature: Jitter tolerance (AES3-4-2009)
  As receiving equipment
  I want to tolerate input jitter
  So that I operate reliably with real-world signals

  Scenario: Receiver jitter tolerance
    Given equipment receiving AES3 audio input
    When input jitter injected per AES3-4-2009 test
    Then receiver maintains lock without errors
    And no sample slips occur
    And jitter tolerance meets AES3-4 specifications
```

**Dependencies**:
- StR-PERF-001 (Frequency accuracy foundation)
- StR-PERF-002 (Lock stability under jitter)
- Clock architecture (PLL loop filter design, VCO quality)

**Risks**:
- **Risk**: Jitter measurement equipment availability → **Mitigation**: Software-based estimation, test lab partnerships
- **Risk**: Platform-specific jitter sources → **Mitigation**: Characterization per platform, mitigation guidelines

**Validation Method**: Time interval analyzer measurements, AES3-4 jitter tolerance testing, eye diagram analysis

---

## StR-PERF-005: Real-Time Processing Latency and Footprint

**ID**: StR-PERF-005  
**Priority**: High (P1)  
**Category**: Embedded Performance  
**Source**: Embedded Systems Developers, Real-Time Constraints  

**Stakeholder Statement**:
> "We need the library to meet real-time constraints on embedded systems like ARM Cortex-M7, with predictable latency, small memory footprint, and no dynamic allocation."

**Requirement**:
The solution shall operate within real-time constraints on ARM Cortex-M7 platforms with <10 µs processing latency per DARS frame, <64 KB static memory footprint, and no dynamic memory allocation or blocking operations.

**Rationale**:
- Embedded audio systems have strict real-time deadlines
- Audio buffer underruns cause audible artifacts
- Dynamic allocation is non-deterministic and forbidden in many real-time systems
- Memory is constrained on embedded platforms

**Numerical Requirements**:

| Metric | Target | Platform | Rationale |
|--------|--------|----------|-----------|
| **Processing Latency** | <10 µs | ARM Cortex-M7 @216 MHz | Sub-sample at 96 kHz (10.4 µs) |
| **Static Memory** | <64 KB | Embedded systems | Typical SRAM constraints |
| **Dynamic Allocation** | Zero | All platforms | Real-time safety |
| **CPU Utilization** | <10% | ARM Cortex-M7 | Leave headroom for app |

**Real-Time Safety Requirements**:
- Static memory allocation only (no malloc/free)
- No blocking operations (no mutexes, no I/O waits)
- Bounded execution time (WCET analysis possible)
- Interrupt-safe design (atomic operations where needed)

**Success Criteria**:
- ✅ DARS frame processing <10 µs on ARM Cortex-M7 @216 MHz
- ✅ Static memory footprint <64 KB (measured, not estimated)
- ✅ Zero dynamic allocations (verified with instrumentation)
- ✅ CPU utilization <10% during steady-state operation
- ✅ Deterministic behavior (no unbounded loops)

**Acceptance Criteria**:
```gherkin
Feature: Processing latency on embedded platforms
  As an embedded audio system
  I want predictable low latency
  So that real-time deadlines are met

  Scenario: DARS frame processing latency at 48 kHz
    Given ARM Cortex-M7 platform at 216 MHz
    And DARS frame generation at 48 kHz
    When processing time measured with timer
    Then worst-case execution time (WCET) is <10 µs
    And typical execution time <5 µs
    And latency measured over 10,000 frames shows no outliers
    And processing time independent of phase/jitter

  Scenario: Video synchronization processing
    Given video-referenced DARS at 48 kHz @ 30 Hz
    When video sync alignment processed
    Then processing completes within 10 µs budget
    And no frame deadlines missed over 1-hour test

Feature: Memory footprint constraints
  As an embedded system with limited RAM
  I want small static footprint
  So that other system functions have memory

  Scenario: Static memory measurement
    Given library compiled for ARM Cortex-M7
    When memory usage analyzed with linker map
    Then .bss + .data sections total <64 KB
    And stack usage <8 KB (measured with stack analyzer)
    And no heap usage (.heap section is zero)

  Scenario: Zero dynamic allocation
    Given library operating in steady state
    When instrumented malloc/free wrappers monitor calls
    Then zero allocations occur after initialization
    And zero deallocations occur
    And allocation instrumentation confirms real-time safety

Feature: CPU utilization on embedded platforms
  As an embedded system running multiple tasks
  I want library to use <10% CPU
  So that application has processing headroom

  Scenario: Steady-state CPU utilization at 48 kHz
    Given ARM Cortex-M7 at 216 MHz
    And DARS generation active at 48 kHz
    When CPU profiler measures utilization
    Then CPU usage is <10% average
    And no CPU usage spikes >20%
    And profiler shows deterministic execution pattern

Feature: Real-time safety validation
  As a real-time system architect
  I want to verify real-time safety
  So that system is certifiable

  Scenario: Static analysis verification
    Given library source code
    When MISRA-C static analyzer runs
    Then no dynamic allocation violations
    And no unbounded loops detected
    And no blocking operations identified
    And worst-case execution time (WCET) is calculable
```

**Dependencies**:
- StR-FUNC-002 (HAL design must support zero-copy operations)
- Compiler optimization settings (–O2 or –O3)
- Platform HAL implementation efficiency

**Risks**:
- **Risk**: Platform HAL overhead affects latency → **Mitigation**: HAL validation tool, profiling
- **Risk**: Compiler optimizations change behavior → **Mitigation**: Regression testing, CI verification

**Validation Method**: Performance profiling, memory analysis tools, real-time tracing, WCET analysis tools

---

**Summary**: Performance requirements specify exact numerical targets for timing precision (±1/±10 ppm), capture range (±2/±50 ppm), phase relationships (±5%/±25%), jitter (<10 ns RMS), and embedded constraints (<10 µs latency, <64 KB footprint) derived directly from AES-11-2009 Sections 5, 6, and Table 2.
