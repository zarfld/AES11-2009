---
applyTo: "**"
---

# AES-11 Standards Implementation AI Coding Instructions

## CRITICAL: Hardware-Agnostic Digital Audio Synchronization Implementation

The Standards layer (`lib/Standards/`) implements the **AES-11 Digital Audio Reference Signal (DARS) protocol** that MUST remain completely hardware and vendor agnostic for maximum portability across audio equipment manufacturers.

## Working Principles for AES-11 Implementation

- **Understand AES-11 protocol before implementing** - study AES-11-2009 specification and DARS timing requirements thoroughly
- **No Fake, No Stubs, no Simulations** - implement complete DARS protocol logic, not simplified placeholders
- **No implementation-based assumptions** - use AES-11 specification or analysis results only (ask if required)
- **No false advertising** - prove and ensure AES-11 compliance and correctness
- **Never break protocol APIs** in ways that violate AES-11 standards - prefer fixing implementation instead
- **Prevent dead code or orphan files** - fix code rather than creating new versions, ensure ALL code compiles
- **Real-time audio constraints** - meet Grade 1 (±1 ppm) and Grade 2 (±10 ppm) timing precision requirements
- **Always reference ### Documentation Requirements with Copyright Compliance

### Function Documentation with AES-11 Context
```cpp
/**
 * @brief Parse AES-11 DARS (Digital Audio Reference Signal) frame from audio stream
 * 
 * Validates and extracts DARS timing information according to AES-11-2009
 * specification section 5.1. Performs integrity checks including frame
 * preamble validation and timing reference point detection.
 *
 * @param audio_data Raw AES3 audio data containing DARS
 * @param data_length Length of audio data in bytes
 * @param dars_info Output buffer for parsed DARS information
 * @return 0 on success, negative error code on failure
 * 
 * @note Implements AES-11-2009 Section 5.1 DARS requirements
 * @see AES-11-2009, Section 3.3 "timing reference point"
 * @see AES-11-2009, Section 5.1.3 "DARS identification"
 * 
 * IMPORTANT: This implementation is based on understanding of AES-11-2009
 * specification. No copyrighted content from AES documents is reproduced.
 * Refer to original AES specification for authoritative requirements.
 */
int aes11_parse_dars_frame(const uint8_t* audio_data, 
                           size_t data_length,
                           aes11_dars_info_t* dars_info);
```

### Copyright-Compliant Implementation Comments
```cpp
// ✅ CORRECT - Reference without reproduction
// Implements DARS grade identification per AES-11-2009 Section 5.1.3
#define AES11_DARS_GRADE_1 0x01  // As specified in AES-11-2009
#define AES11_DARS_GRADE_2 0x02  // As specified in AES-11-2009

// ❌ WRONG - Reproducing copyrighted content
/*
// DON'T DO THIS - This reproduces copyrighted AES content:
// "A grade-1 DARS is a high-accuracy signal intended for synchronizing 
//  systematically a multiple-studio complex and may also be used for 
//  a stand-alone studio."
// This is direct reproduction of AES copyrighted text!
*/

// ✅ CORRECT - Original implementation based on specification understanding
struct aes11_dars_info {
    uint8_t grade;               // AES-11-2009 Table (byte 4, bits 0-1)
    uint32_t sample_frequency;   // AES-11-2009 Section 5.1.6
    uint64_t timing_ref_point;   // AES-11-2009 Section 3.3
    uint8_t channel_status[24];  // AES3 channel status data
    // ... implementation based on specification understanding
};
```

### Standards Compliance Notes with Copyright Respect
- **Always reference specific AES section and table numbers** when implementing protocol features
- **NEVER reproduce copyrighted text** from specifications in comments or documentation  
- **Document video sync extensions separately** with SMPTE specification references (respecting SMPTE copyright)
- **Include timing requirements** with original implementation based on specification understanding
- **Specify sample rate handling** for AES5 standard sampling frequencies
- **Use only factual technical information** that is not subject to copyright (constants, field sizes, etc.)
- **Validate all protocol fields** with range checks or masks from the AES specification
- **Every function must have documentation** explaining purpose, parameters, return values, and AES context
- **No duplicate or redundant implementations** to avoid inconsistencies and confusion
- **Prevent downgrades** - fix rather than delete existing protocol functionality
- **No ad-hoc file copies** (e.g., *_fixed, *_new, *_correct) - refactor in place step-by-step

## Core Principle: Standards-Only Implementation

- **ONLY AES-11/AES3 protocol logic** - no hardware-specific code
- **Hardware agnostic** - compilable without any vendor drivers or hardware headers
- **Mockable/testable** - runnable without physical hardware present
- **Real-time safe** - meets professional audio timing constraints (Grade 1: ±1 ppm, Grade 2: ±10 ppm)
- **Dependency injection pattern** - receives hardware abstraction via interfaces/function pointers

### FORBIDDEN in Standards Layer
```cpp
// ❌ NEVER include vendor-specific headers
#include "asio_audio_hal.h"               // NO - ASIO specific
#include "../../vendor_audio/include/*.h" // NO - Vendor specific  
#include "../../common/hal/audio_hal.h"   // NO - HAL abstraction
#include <alsa/asoundlib.h>               // NO - Linux ALSA specific
#include <windows.h>                      // NO - Windows specific
#include <CoreAudio/CoreAudio.h>          // NO - macOS specific
```

### REQUIRED in Standards Layer
```cpp
// ✅ Only AES standards and audio protocol headers
#include "aes3.h"                         // AES3 digital audio interface
#include "aes11_dars.h"                   // AES-11 DARS protocol
#include "aes5_sampling.h"                // AES5 sampling frequencies
#include "smpte_timecode.h"               // SMPTE timecode (for video sync)
```

## Architecture Compliance

### Standards Layer Responsibility
- **DARS protocol state machines** (AES-11 synchronization, Grade 1/2 timing)
- **AES3 frame format handling** (parsing, validation, construction)
- **Timing calculations** (sample rate accuracy, phase relationships, jitter analysis)
- **AES-11 defined structures** and constants (DARS info, channel status, timing tolerances)
- **AES-11 compliance validation** (Grade 1: ±1 ppm, Grade 2: ±10 ppm)

### Interface Pattern for Hardware Access
```cpp
// ✅ CORRECT: Dependency injection interface for audio hardware abstraction
typedef struct {
    int (*send_audio_frame)(const aes3_frame_t* frame);
    int (*receive_audio_frame)(aes3_frame_t* frame);
    uint64_t (*get_sample_time_ns)(void);
    int (*set_sample_rate)(uint32_t sample_rate_hz);
    int (*get_timing_reference)(aes11_timing_ref_t* timing_ref);
} audio_interface_t;

// AES-11 protocol implementation receives interface
int aes11_dars_init(const audio_interface_t* audio_interface);
```

### Hardware Bridge (Service Layer Responsibility)
```cpp
// This code belongs in SERVICE layer (e.g., audio driver), NOT Standards
static int asio_send_audio_frame(const aes3_frame_t* frame) {
    return asio_hal_send_frame(frame);  // ASIO-specific call
}

static audio_interface_t asio_interface = {
    .send_audio_frame = asio_send_audio_frame,
    .receive_audio_frame = asio_receive_audio_frame,
    .get_sample_time_ns = asio_get_sample_time,
    // ... other ASIO-specific implementations
};

// Service layer bridges Standards to ASIO Hardware
aes11_dars_init(&asio_interface);
```

## AES Standards Reference

### CRITICAL: Use Authoritative Standards Documents
When implementing AES-11 protocol, reference these authoritative documents via MCP-Server:

#### Primary AES-11 Standard:
- `AES 11-2009 (R2014)-en.pdf` - **AES11-2009 Digital Audio Reference Signals (PRIMARY REFERENCE)**

#### Supporting AES Audio Engineering Standards:
- `AES 3-1-2009 (R2014)-en.pdf` - AES3-1-2009 Digital Audio Interface Part 1 - Frame format foundation
- `AES 3-2-2009 (R2014)-en.pdf` - AES3-2-2009 Digital Audio Interface Part 2 - Channel status data
- `AES 3-3-2009 (R2014)-en.pdf` - AES3-3-2009 Digital Audio Interface Part 3 - Preamble definitions
- `AES 3-4-2009 (R2014)-en.pdf` - AES3-4-2009 Digital Audio Interface Part 4 - Jitter specifications
- `AES 5-2018-en.pdf` - AES5-2018 Preferred Sampling Frequencies - Sample rate standards

#### Video Synchronization Standards (Referenced by AES-11):
- `SMPTE 318M-1999` - Synchronization of 59.94 or 50 Hz video and audio systems
- `SMPTE RP168` - Vertical Interval Switching Point definition

#### Optional Integration Standards:
- `AES 67-2018-en.pdf` - AES67-2018 Audio-over-IP (may use AES-11 for sync)
- `IEEE 1588-2019-en.pdf` - Precision Time Protocol (alternative sync method)

**CRITICAL COPYRIGHT AND LICENSING RESTRICTIONS**:

⚠️ **ABSOLUTELY FORBIDDEN**:
- **DO NOT copy any text, tables, figures, or content** directly from IEEE, AES, or AVnu documents
- **DO NOT reproduce specification text** verbatim in source code comments
- **DO NOT include copyrighted tables or figures** in repository documentation
- **DO NOT paste specification sections** into code or documentation files
- **DO NOT create derivative works** that reproduce substantial portions of standards

✅ **PERMITTED USAGE**:
- **Reference document sections** by number only (e.g., "See AES-11-2009, Section 5.1.3")
- **Implement protocol logic** based on understanding of specifications
- **Create original code** that achieves compliance with standard requirements
- **Use specification constants and values** in implementation (packet types, field sizes, etc.)
- **Reference via MCP-Server** for compliance verification during development only

**LICENSE NOTICE**: These documents are referenced for compliance verification and implementation guidance only. All content remains under copyright of respective standards organizations (AES, SMPTE, IEEE, ITU). Any reproduction, distribution, or derivative works require proper licensing from copyright holders.

**Usage Pattern**: When implementing standards-related code, reference these documents via MCP-Server to ensure:
- Protocol message format compliance
- State machine behavior correctness  
- Timing requirement adherence
- AES audio format compliance
- Video synchronization compliance
- DARS timing precision requirements

### Protocol Compliance Requirements
```cpp
// ✅ Use authoritative constants from AES-11 specification
#include "aes11_constants.h"
uint8_t dars_grade = AES11_DARS_GRADE_1;  // As defined in AES-11-2009 Section 5.1.3
uint32_t sample_rate = AES5_SAMPLE_RATE_48KHZ;  // As defined in AES5

// ❌ NEVER use hardcoded values
uint8_t dars_grade = 0x01;  // NO - not standards compliant
uint32_t sample_rate = 48000;  // NO - use AES5 constants
```

## Testing Approach

### Unit Testing (Hardware-Independent)
```cpp
// ✅ Mock network interface for testing
static int mock_send_packet(const void* packet, size_t length) {
    // Record packet for verification
    return 0;
}

static network_interface_t mock_interface = {
    .send_packet = mock_send_packet,
    // ... other mock implementations
};

// Test protocol logic without hardware
void test_aes11_dars_generation(void) {
    aes11_dars_init(&mock_interface);
    // ... test DARS protocol behavior
}
```

### Integration Testing (Service Layer Responsibility)
Integration with real hardware happens in the Service Layer, NOT in Standards.

## Common Violations to Avoid

### ❌ Direct Hardware Calls
```cpp
// WRONG - Standards calling Intel HAL directly
intel_hal_result_t result = intel_hal_send_packet(packet, length);
```

### ❌ OS-Specific Code
```cpp
// WRONG - OS-specific networking in Standards
#ifdef _WIN32
    SOCKET sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
#else
    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
#endif
```

### ❌ Hardware Assumptions
```cpp
// WRONG - Assuming Intel hardware capabilities
if (intel_hal_supports_ptp()) {  // Hardware-specific assumption
    enable_hardware_timestamping();
}
```

### ✅ Correct Abstraction
```cpp
// CORRECT - Protocol logic with abstracted capabilities
if (net_interface->capabilities & NETWORK_CAP_HARDWARE_TIMESTAMP) {
    enable_precision_timing();  // Protocol behavior, not hardware call
}
```

## Protocol Implementation Guidelines

### AES-11 DARS (Digital Audio Reference Signal)
- Implement DARS state machines according to AES-11-2009 Section 5
- Support Grade 1 (±1 ppm) and Grade 2 (±10 ppm) frequency accuracy per Section 5.2
- Handle timing reference point detection per Section 3.3 (X/Z preamble transitions)
- Validate DARS identification in channel status byte 4, bits 0-1 per Section 5.1.3
- Support all AES5 sampling frequencies (32, 44.1, 48, 96 kHz) per Section 5.1.6
- Implement capture range requirements: ±2 ppm (Grade 1), ±50 ppm (Grade 2)

### AES3 Digital Audio Interface Integration
- Parse AES3 frame structure for DARS extraction per AES3 specifications
- Handle channel status data processing for DARS metadata
- Implement preamble detection (X, Y, Z) for timing reference points
- Support both audio and non-audio DARS content per Section 5.1.4
- Validate frame alignment within ±25% tolerance per Section 5.3.1.2

### Video Synchronization Support
- Implement video-to-audio reference relationships per Section 4.5 and Table 1
- Support integer ratio video systems (synchronous locking) per Section 4.5.2  
- Handle non-integer ratio systems with ±1 sample tolerance per Section 4.5.3
- Align DARS to video sync pulse half-amplitude point per Section 5.3.4
- Support NTSC/PAL frame rates including 30/1.001 Hz variants

## Clean Submit Rules for Standards

- **Each commit compiles and passes AES-11 compliance checks**
- **Small, single-purpose, reviewable diffs** (no WIP noise)
- **No dead or commented-out code** - remove unused protocol implementations
- **Run formatter and static analysis** before commit
- **Update documentation and reference AES spec sections** in commit messages
- **Use feature flags for incremental protocol changes** when they risk breaking AES-11 compliance
- **Reference exact AES section numbers** (e.g., "Implements AES-11-2009 Section 5.1.3")

## Documentation Requirements

### Function Documentation with AES-11 Context
```cpp
/**
 * @brief Generate AES-11 DARS timing reference from video sync pulse
 * 
 * Creates DARS timing reference point according to AES-11-2009 section 5.3.4.
 * Aligns X/Z preamble to half-amplitude point of video sync pulse per
 * specification requirements for video-referenced synchronization.
 *
 * @param video_sync_pulse Video synchronization pulse input
 * @param sample_rate Target audio sample rate (32, 44.1, 48, 96 kHz)
 * @param dars_output Generated DARS reference signal output
 * @return 0 on success, negative error code on failure
 * 
 * @note Implements AES-11-2009 Section 5.3.4 video reference alignment
 * @see AES-11-2009, Table 1 "Audio-video synchronization"
 * @see AES-11-2009, Section 4.5 "Video referencing"
 */
int aes11_generate_video_referenced_dars(const video_sync_t* video_sync_pulse,
                                        uint32_t sample_rate,
                                        aes11_dars_t* dars_output);
```

### Standards Compliance Notes
- **Always reference specific AES section and table numbers** when implementing protocol features
- **Document video sync extensions separately** with SMPTE specification references
- **Include timing requirements** with microsecond precision for audio timing
- **Specify sample rate handling** for AES5 standard sampling frequencies

## Build System Integration

### CMake Configuration for Standards
The Standards layer should compile independently:
```cmake
# Standards layer has NO hardware dependencies
add_library(aes11_standards STATIC
    src/aes11_dars.c
    src/aes3_interface.c  
    src/aes5_sampling.c
    src/smpte_timecode.c
)

# Only protocol headers, no hardware/HAL
target_include_directories(aes11_standards PUBLIC
    include/
    ../thirdparty/aes3-c/include/
)

# NO hardware libraries linked to Standards
# target_link_libraries(aes11_standards asio_audio_hal)  # ❌ WRONG
```

### Testing Framework Integration
Use unified testing framework for protocol validation:
```cmake
# Protocol compliance tests (hardware-independent)
add_executable(standards_protocol_tests
    tests/test_aes11_compliance.c
    tests/test_dars_state_machines.c
    tests/test_video_sync_extensions.c
    tests/test_aes3_integration.c          # Tests with external AES3
    tests/test_aes5_integration.c          # Tests with external AES5
)

target_link_libraries(standards_protocol_tests 
    aes11_2009                  # Main AES-11 implementation
    aes3_2009                   # External AES3 dependency
    aes5_2018                   # External AES5 dependency  
    standards_common            # Common test utilities
    GoogleTest
    Unity
)
```

## Performance and Correctness

### Memory Management
- **Use static allocation** where possible for real-time audio processing
- **Validate buffer bounds** against AES3 maximum frame sizes
- **Initialize all protocol structures** to prevent undefined behavior
- **Avoid dynamic allocation** in audio processing paths

### Timing Considerations
- **Sample-accurate precision** for DARS timing calculations
- **Microsecond accuracy** for video synchronization alignment
- **Avoid blocking operations** in audio callback paths
- **Meet Grade 1/Grade 2 timing tolerances** (±1 ppm / ±10 ppm)

### Error Handling
- **Return AES-11-compliant error codes** where specifications define them
- **Log protocol violations** with specific AES section references
- **Graceful degradation** for optional timing features
- **Maintain audio continuity** during error recovery

## Mandatory Namespace and Folder Structure

### Generic Structure Definition

**FUNDAMENTAL PATTERN**: All Standards implementations must follow this generic hierarchy:

```
<Organization>/<Standard>/<Subpart>/<Version>/
```

**Components Definition:**
- **Organization**: Standards body (IEEE, AVnu, AES, ITU, IETF, etc.)
- **Standard**: Standard family or main specification number  
- **Subpart**: Sub-specifications, working groups, or functional areas
- **Version**: Year of publication or version number

**Translation Rules:**
- **Folder Structure**: Preserve dots and organization naming (`IEEE/802.1/AS/2021/`)
- **Namespace Structure**: Convert dots to underscores, preserve organization (`IEEE::_802_1::AS::_2021`)
- **File Structure**: Follow folder pattern with appropriate extensions

### Concrete Examples by Organization

#### Audio Engineering Society Structure (Primary)
```
AES/<Standard>/<Subpart>/<Version>/
├── AES/AES11/2009/              # AES11-2009 Digital Audio Reference Signals (PRIMARY)
├── AES/AES3/2009/               # AES3-2009 digital audio interface (foundation)
├── AES/AES5/2018/               # AES5-2018 sampling frequencies
├── AES/AES67/2018/              # AES67-2018 audio-over-IP (integration)
└── AES/AES70/2021/              # AES70-2021 device control (OCA)
```

#### Video Standards Structure (Referenced by AES-11)
```
SMPTE/<Standard>/<Version>/
├── SMPTE/318M/1999/             # Video/audio synchronization
└── SMPTE/RP168/                 # Vertical Interval Switching Point
```

#### Optional Integration Standards Structure  
```
IEEE/<Standard>/<Version>/
└── IEEE/1588/2019/              # IEEE 1588-2019 (PTPv2 - alternative sync)
```

#### ITU-T Structure (if needed)
```
ITU/<Series>/<Standard>/<Version>/
├── ITU/G/G.8275.1/2016/         # ITU-T G.8275.1-2016 telecom PTP
└── ITU/G/G.8275.2/2017/         # ITU-T G.8275.2-2017 telecom PTP
```

### Required Directory Structure
```
lib/Standards/
├── AES/                       # Audio Engineering Society (PRIMARY)
│   └── AES11/                 # AES11 Digital Audio Reference Signals (THIS REPO)
│       └── 2009/              # AES11-2009 specification (PRIMARY IMPLEMENTATION)
│           ├── core/          # Core DARS protocol and state machines
│           ├── dars/          # DARS generation and validation
│           ├── sync/          # Synchronization methods and timing
│           ├── video/         # Video reference synchronization
│           └── conformity/    # AES-11 conformance testing
├── Common/                    # Cross-standard utilities (THIS REPO)
│   ├── utils/                 # Shared audio utilities
│   ├── testing/               # Common testing frameworks
│   ├── interfaces/            # Hardware abstraction interfaces
│   └── timing/                # Cross-standard timing utilities
└── Documentation/             # Standards documentation (THIS REPO)
    ├── conformance/           # Conformance test specifications
    ├── interop/              # Interoperability guidelines
    └── examples/             # Protocol usage examples

# External Dependencies (Separate Repositories):
# - AES3-2009: https://github.com/zarfld/AES3-2009.git (READY)
# - AES5-2018: https://github.com/zarfld/AES5-2018.git (READY)  
# - IEEE_1588_2019: https://github.com/zarfld/IEEE_1588_2019.git (IN DEVELOPMENT)
# - SMPTE standards: Future external repositories as needed
```

### Required C++ Namespace Structure Following Generic Pattern

**NAMESPACE TRANSLATION RULE**: `<Organization>::<Standard_with_underscores>::<Subpart>::<Version_with_underscores>`

**Examples of Translation:**
- Folder: `IEEE/802.1/AS/2021/` → Namespace: `IEEE::_802_1::AS::_2021`
- Folder: `IEEE/1722.1/2021/` → Namespace: `IEEE::_1722_1::_2021`  
- Folder: `AVnu/Milan/v1.2/` → Namespace: `AVnu::Milan::v1_2`
- Folder: `AES/AES67/2018/` → Namespace: `AES::AES67::_2018`

```cpp
// ✅ CORRECT namespace hierarchy following generic pattern
namespace AES {                 // Organization: Audio Engineering Society (PRIMARY)
    namespace AES11 {            // Standard: AES11 (Digital Audio Reference Signals) - THIS REPO
        namespace _2009 {        // Version: 2009 (year)
            // AES11-2009 DARS implementation - PRIMARY FOCUS
            namespace core {
                class DARSProtocol;
                class SynchronizationManager;
                class TimingReferencePoint;
            }
            namespace dars {
                class DARSGenerator;
                class DARSValidator;
                class GradeClassification;  // Grade 1/Grade 2
            }
            namespace sync {
                class VideoReferenceSync;
                class GPSReferenceSync;
                class AudioInputSync;
            }
            namespace video {
                class VideoSyncAlignment;    // Section 5.3.4
                class FrameRateHandler;     // Table 1 relationships
            }
            namespace conformity {
                class AES11ConformanceTest;
                class TimingToleranceTest;
            }
        }
    }
    
    // External Dependencies (Separate Repositories):
    // AES3::_2009 from https://github.com/zarfld/AES3-2009.git (READY)
    // AES5::_2018 from https://github.com/zarfld/AES5-2018.git (READY)
    
} // namespace AES

namespace SMPTE {               // Organization: SMPTE (Video standards)
    namespace _318M {           // Standard: 318M (Video/audio sync)
        namespace _1999 {       // Version: 1999 (year)
            class VideoAudioSynchronization;
        }
    }
    namespace RP168 {           // Standard: RP168 (Vertical Interval)
        class VerticalIntervalSwitching;
    }
} // namespace SMPTE

namespace IEEE {                // Organization: IEEE (Optional integration)
    namespace _1588 {           // Standard: 1588 (PTPv2 - alternative sync)
        namespace _2019 {       // Version: 2019 (year)
            class PTPv2Protocol;    // Alternative to AES-11 DARS
        }
    }
} // namespace IEEE

namespace ITU {                  // Organization: ITU-T (if needed)
    namespace G {                // Standard: G-series recommendations
        namespace G8275_1 {      // Subpart: G.8275.1 (dots→underscores)
            namespace _2016 {    // Version: 2016 (year)
                // ITU-T G.8275.1-2016 telecom PTP
            }
        }
    }
} // namespace ITU

namespace Common {               // Cross-organization utilities
    namespace interfaces {
        // Hardware abstraction interfaces - ONLY in Common namespace
        class NetworkInterface;
        class TimerInterface;
        class ClockInterface;
    }
    namespace utils {
        // Cross-standard utilities
        class PacketParser;
        class CRCCalculator;
    }
    namespace testing {
        // Common testing frameworks
        class TestFrameworkBase;
    }
} // namespace Common
```

### File Naming Conventions Following Generic Pattern

**PATTERN**: `<Organization>/<Standard>/<Subpart>/<Version>/<functional_area>/<file_name>.<ext>`

```cpp
// ✅ CORRECT file naming following generic pattern

// AES-11 Implementation (THIS REPO):
lib/Standards/AES/AES11/2009/core/dars_protocol.hpp                     // AES::AES11::_2009::core
lib/Standards/AES/AES11/2009/core/dars_protocol.cpp
lib/Standards/AES/AES11/2009/core/synchronization_manager.hpp           // AES::AES11::_2009::core
lib/Standards/AES/AES11/2009/core/timing_reference_point.hpp            // AES::AES11::_2009::core

lib/Standards/AES/AES11/2009/dars/dars_generator.hpp                    // AES::AES11::_2009::dars
lib/Standards/AES/AES11/2009/dars/dars_generator.cpp
lib/Standards/AES/AES11/2009/dars/dars_validator.hpp                    // AES::AES11::_2009::dars
lib/Standards/AES/AES11/2009/dars/grade_classification.hpp              // AES::AES11::_2009::dars

lib/Standards/AES/AES11/2009/sync/video_reference_sync.hpp              // AES::AES11::_2009::sync
lib/Standards/AES/AES11/2009/sync/gps_reference_sync.hpp                // AES::AES11::_2009::sync
lib/Standards/AES/AES11/2009/sync/audio_input_sync.hpp                  // AES::AES11::_2009::sync

lib/Standards/AES/AES11/2009/video/video_sync_alignment.hpp             // AES::AES11::_2009::video
lib/Standards/AES/AES11/2009/video/frame_rate_handler.hpp               // AES::AES11::_2009::video

lib/Standards/AES/AES11/2009/conformity/aes11_conformance_test.hpp      // AES::AES11::_2009::conformity
lib/Standards/AES/AES11/2009/conformity/timing_tolerance_test.hpp       // AES::AES11::_2009::conformity

// Common utilities (THIS REPO):
lib/Standards/Common/interfaces/audio_interface.hpp                     // Common::interfaces
lib/Standards/Common/utils/audio_parser.hpp                            // Common::utils
lib/Standards/Common/testing/test_framework_base.hpp                    // Common::testing
lib/Standards/Common/timing/cross_standard_timing.hpp                   // Common::timing

// External Dependencies (Separate Repositories):
// AES3-2009: https://github.com/zarfld/AES3-2009.git (READY)
//   - AES::AES3::_2009::frames::AES3Frame
//   - AES::AES3::_2009::preambles::PreambleDetector
//   - AES::AES3::_2009::channel::ChannelStatusProcessor

// AES5-2018: https://github.com/zarfld/AES5-2018.git (READY)  
//   - AES::AES5::_2018::rates::StandardSamplingRates
//   - AES::AES5::_2018::ratios::SampleRateRelationships

// IEEE_1588_2019: https://github.com/zarfld/IEEE_1588_2019.git (IN DEVELOPMENT)
//   - IEEE::_1588::_2019::core::PTPv2Protocol

// Future SMPTE repositories as needed for video synchronization
```

### Header Guard and Include Conventions Following Actual Pattern
```cpp
// ✅ CORRECT header guards following IEEE namespace structure
#ifndef IEEE_1722_1_2021_AEM_ENTITY_MODEL_H
#define IEEE_1722_1_2021_AEM_ENTITY_MODEL_H

// ✅ CORRECT include structure - relative paths from IEEE namespace
#include "../descriptors/entity_descriptor.h"
#include "../../../../Common/interfaces/network_interface.h"

namespace IEEE {
namespace _1722_1 {
namespace _2021 {
namespace aem {

class EntityModel {
    // IEEE 1722.1-2021 compliant implementation
};

} // namespace aem
} // namespace _2021
} // namespace _1722_1
} // namespace IEEE

#endif // IEEE_1722_1_2021_AEM_ENTITY_MODEL_H
```

### Correct Include Patterns for Cross-Standard Dependencies
```cpp
// ✅ CORRECT - IEEE standards can reference each other
#include "../../1722/2016/avtp/avtp_packet.h"        // AVDECC using AVTP
#include "../../802.1/AS/2021/core/time_sync.h"      // AVDECC using gPTP

// ✅ CORRECT - Common utilities accessible to all standards
#include "../../../Common/interfaces/network_interface.h"
#include "../../../Common/utils/packet_parser.h"

// ❌ WRONG - No hardware-specific includes in IEEE namespace
// #include "../../../../../intel_avb/include/intel_hal.h"  // NO!
// #include "../../../../../common/hal/network_hal.h"       // NO!
```

### Cross-Standard Reuse and Dependencies

**MANDATORY RULE**: When an AES standard references or builds upon another AES standard, **ALWAYS reuse the existing implementation** rather than creating redundant code.

#### Examples of Required Cross-Standard Reuse:

**AES-11 (DARS) Dependencies:**
```cpp
namespace AES {
namespace AES11 {
namespace _2009 {
namespace dars {

// ✅ CORRECT - Reuse AES3 frame processing
#include "../../../../AES3/2009/frames/aes3_frame.h"
using AES::AES3::_2009::frames::AES3Frame;

// ✅ CORRECT - Reuse AES5 sampling rates  
#include "../../../../AES5/2018/rates/standard_sampling_rates.h"
using AES::AES5::_2018::rates::StandardSamplingRates;

class DARSGenerator {
    // DARS is transported in AES3 frames
    AES::AES3::_2009::frames::AES3Frame create_dars_frame() {
        // Reuse AES3 frame format, don't reimplement
        return AES::AES3::_2009::frames::AES3Frame::create_dars_frame();
    }
    
    // DARS uses AES5 standard sampling rates
    uint32_t get_sample_rate() {
        // Reuse AES5 rates, don't hardcode values
        return AES::AES5::_2018::rates::StandardSamplingRates::get_48khz();
    }
};

} // namespace dars
} // namespace _2009
} // namespace AES11
} // namespace AES
```

**AES67 (Audio-over-IP) Dependencies:**
```cpp
namespace AES {
namespace AES67 {
namespace _2018 {
namespace sync {

// ✅ CORRECT - Reuse AES-11 DARS for synchronization
#include "../../../../AES11/2009/dars/dars_generator.h"

class NetworkSynchronization {
    uint64_t get_reference_time() {
        // Reuse AES-11 DARS timing, don't reimplement sync
        return AES::AES11::_2009::dars::DARSGenerator::get_timing_reference();
    }
};

} // namespace sync
} // namespace _2018
} // namespace AES67
} // namespace AES
```

#### Forbidden Redundant Implementations:
```cpp
// ❌ WRONG - Reimplementing existing AES standards
namespace AES {
namespace AES11 {
namespace _2009 {

// DON'T DO THIS - AES3 already exists in AES::AES3
class CustomAudioFrame {  // NO - use AES::AES3::_2009::frames::AES3Frame
    // ... redundant AES3 implementation
};

// DON'T DO THIS - AES5 already exists in AES::AES5  
class CustomSampleRates {  // NO - use AES::AES5::_2018::rates::StandardSamplingRates
    // ... redundant sampling rate implementation
};

} // namespace _2009
} // namespace AES11
} // namespace AES
```

#### Cross-Standard Dependency Rules:
1. **AES Audio Hierarchy** (higher layers depend on lower layers):
   - **Application Layer**: AES67 (Audio-over-IP), AES70 (Device Control)
   - **Synchronization Layer**: AES-11 (DARS) 
   - **Transport Layer**: AES3 (Digital Audio Interface)
   - **Foundation Layer**: AES5 (Sampling Frequencies)

2. **Dependency Direction**: 
   - ✅ **AES-11 CAN depend on AES3 and AES5**
   - ✅ **AES67 CAN depend on AES-11, AES3, and AES5**
   - ❌ **AES3 CANNOT depend on AES-11 or AES67**
   - ❌ **AES5 CANNOT depend on higher-layer AES standards**

3. **Extension Standards**:
   - ✅ **AES67 CAN depend on IEEE standards it references**
   - ✅ **Multiple standard versions CAN coexist** (2009, 2018, etc.)

#### CMake Dependencies for Cross-Standard Reuse:
```cmake
# ✅ CORRECT - Respect AES layering in CMake dependencies
target_link_libraries(aes11_2009
    aes3_2009                   # External: AES3 frame format and preambles
    aes5_2018                   # External: AES5 sampling rates  
    standards_common            # All can use Common utilities
)

target_link_libraries(ieee_1722_2016
    ieee_802_1_as_2021          # AVTP depends on gPTP for timing
    standards_common
)

target_link_libraries(avnu_milan_v12
    ieee_1722_1_2021            # Milan extends IEEE 1722.1
    ieee_1722_2016              # Milan may use AVTP directly
    ieee_802_1_as_2021          # Milan requires precise timing
    standards_common
)

# ❌ WRONG - Violates layering hierarchy
# target_link_libraries(aes3_2009
#     aes11_2009              # NO - AES3 cannot depend on AES-11
# )
```

This approach ensures:
- **No code duplication** across IEEE standards
- **Proper architectural layering** following IEEE specifications  
- **Consistent behavior** when standards reference each other
- **Maintainable codebase** with single source of truth for each protocol feature

### Forbidden Namespace Violations - Corrected
```cpp
// ❌ WRONG - mixing standards with hardware (corrected understanding)
namespace AES {
namespace asio {              // NO - hardware vendor in AES namespace
    class ASIOAudioInterface;
}
}

// ❌ WRONG - OS-specific namespaces in AES standards
namespace AES {
namespace windows {            // NO - OS specific in AES namespace
    class WASAPIInterface;
}
}

// ❌ WRONG - implementation details in AES namespace  
namespace AES {
namespace AES11 {
namespace _2009 {
    class WindowsAudioImpl;  // NO - implementation detail, not protocol
}
}
}

// ✅ CORRECT - AES standards are pure protocol implementations
namespace AES {
namespace AES11 {
namespace _2009 {
namespace dars {
    class DARSGenerator;        // YES - pure AES-11-2009 protocol
}
}
}
}
```

### CMake Integration with External Dependencies
```cmake
# ✅ CORRECT CMake structure for AES-11 with external dependencies

# AES-11 Implementation (THIS REPO)
add_library(aes11_2009 STATIC
    AES/AES11/2009/core/dars_protocol.cpp
    AES/AES11/2009/core/synchronization_manager.cpp
    AES/AES11/2009/dars/dars_generator.cpp
    AES/AES11/2009/dars/dars_validator.cpp
    AES/AES11/2009/sync/video_reference_sync.cpp
    AES/AES11/2009/video/video_sync_alignment.cpp
    AES/AES11/2009/conformity/aes11_conformance_test.cpp
)

# Common utilities (THIS REPO)
add_library(standards_common STATIC
    Common/interfaces/audio_interface.cpp
    Common/utils/audio_parser.cpp
    Common/testing/test_framework_base.cpp
    Common/timing/cross_standard_timing.cpp
)

# External Dependencies via Git Submodules or Package Manager
include(FetchContent)

# AES3-2009 External Repository (READY)
FetchContent_Declare(
    aes3_2009
    GIT_REPOSITORY https://github.com/zarfld/AES3-2009.git
    GIT_TAG        main  # or specific version tag
)
FetchContent_MakeAvailable(aes3_2009)

# AES5-2018 External Repository (READY)  
FetchContent_Declare(
    aes5_2018
    GIT_REPOSITORY https://github.com/zarfld/AES5-2018.git
    GIT_TAG        main  # or specific version tag
)
FetchContent_MakeAvailable(aes5_2018)

# IEEE 1588-2019 External Repository (IN DEVELOPMENT - Optional)
# FetchContent_Declare(
#     ieee_1588_2019
#     GIT_REPOSITORY https://github.com/zarfld/IEEE_1588_2019.git
#     GIT_TAG        main
# )
# FetchContent_MakeAvailable(ieee_1588_2019)

# AES-11 depends on external standards
target_link_libraries(aes11_2009
    aes3_2009                   # External: AES3 frame format and preambles
    aes5_2018                   # External: AES5 sampling rates  
    standards_common            # Internal: Common utilities
    # ieee_1588_2019            # Optional: PTP alternative sync (when ready)
)

target_include_directories(aes11_2009 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/Standards
    ${aes3_2009_SOURCE_DIR}/include        # External AES3 headers
    ${aes5_2018_SOURCE_DIR}/include        # External AES5 headers
)
```

### Documentation Structure Requirements - Corrected
```cpp
/**
 * @file conformity_test_framework.h
 * @brief AES-11-2009 Conformity Testing Framework
 * @namespace AES::AES11::_2009::Testing
 * 
 * Implements conformity testing according to AES-11-2009 specification.
 * This namespace contains all testing functionality for validating AES-11-2009
 * compliance including state machine behavior, timing requirements, and interoperability.
 * 
 * @see AES-11-2009, Section 5.1 "DARS requirements"
 * @see AES-11-2009, Section 5.2 "Grade specifications"
 */
```

### Enforcement Rules - Corrected with Copyright Compliance
1. **AES namespaces are top-level** - not wrapped in other namespace structures
2. **Namespace must match folder structure** exactly (`AES::AES11::_2009` = `AES/AES11/2009/`)
3. **Version numbers use underscores** in namespaces (`_2009`, `_2018`) to avoid conflicts
4. **Standard numbers preserved** in namespaces (`AES11` stays `AES11`, `AES67` stays `AES67`)
5. **No hardware vendors** in AES namespace hierarchy
6. **No OS-specific namespaces** in AES standards
7. **AES layering respected** - higher layer standards can depend on lower layers
8. **Cross-standard utilities** only in Common namespace
9. **Conformance testing** isolated in Testing sub-namespaces
10. **COPYRIGHT COMPLIANCE MANDATORY**:
    - **NEVER reproduce copyrighted specification content** in source code or documentation
    - **Reference specifications by section number only** (e.g., "AES-11-2009, Section 5.1.3")
    - **Implement based on understanding**, not by copying specification text
    - **Use MCP-Server for compliance verification only**, not content reproduction
    - **Respect all copyright holders**: AES, SMPTE, IEEE, ITU, etc.
    - **Include copyright disclaimer** in implementations referencing multiple standards
    - **Document original implementation** that achieves compliance through understanding

### Repository Copyright Policy
```cpp
// ✅ REQUIRED copyright notice for standards-based implementations
/*
 * This file implements protocol functionality based on understanding of:
 * - AES-11-2009 (Digital Audio Reference Signals) - Copyright AES
 * - AES3-2009 (Digital Audio Interface) - Copyright AES
 * - AES5-2018 (Preferred Sampling Frequencies) - Copyright AES
 * - SMPTE 318M-1999 (Video/Audio Synchronization) - Copyright SMPTE
 * 
 * No copyrighted content from these specifications is reproduced.
 * Implementation is original work achieving compliance through
 * understanding of specification requirements.
 * 
 * For authoritative requirements, refer to original specifications
 * available from respective standards organizations.
 */
```

This structure ensures clear separation of AES standards versions, prevents architectural violations, and maintains the hardware-agnostic principle while following the actual implementation pattern used in the codebase.

This architecture ensures the Standards layer remains pure, testable, reusable across different audio hardware platforms, and maintains strict AES-11 compliance while following professional audio development principles.

## ⚠️ MANDATORY: YAML Front Matter Schema Compliance

**CRITICAL ENFORCEMENT**: All specification files MUST use EXACT YAML front matter format per authoritative schemas.

**Authoritative Schema Sources** (SINGLE SOURCE OF TRUTH):
- Requirements: `spec-kit-templates/schemas/requirements-spec.schema.json`  
- Architecture: `spec-kit-templates/schemas/architecture-spec.schema.json`
- Design: `spec-kit-templates/schemas/ieee-design-spec.schema.json`
- Phase Gates: `spec-kit-templates/schemas/phase-gate-validation.schema.json`

**ZERO TOLERANCE POLICY**: 
- ❌ DO NOT modify schemas to fit incorrect front matter
- ❌ DO NOT use alternative YAML formats  
- ❌ DO NOT use full standard names where schemas expect short patterns
- ✅ ALWAYS reference authoritative schema files for format
- ✅ ALWAYS validate against schemas before submitting
- ✅ ALWAYS use phase-specific copilot-instructions for examples

**CI ENFORCEMENT**: Validation failures will block CI pipeline and prevent merges. There are no exceptions to schema compliance.