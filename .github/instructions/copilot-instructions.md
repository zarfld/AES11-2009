---
applyTo: '**'
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
- **Always reference specific IEEE section and table numbers** when implementing protocol features
- **NEVER reproduce copyrighted text** from specifications in comments or documentation
- **Document Milan extensions separately** with AVnu specification references (respecting AVnu copyright)
- **Include timing requirements** with original implementation based on specification understanding
- **Specify endianness handling** based on IEEE network byte order requirements
- **Use only factual technical information** that is not subject to copyright (constants, field sizes, etc.)ification sections** when implementing protocol features
- **Validate all protocol fields** with range checks or masks from the IEEE specification
- **Every function must have documentation** explaining purpose, parameters, return values, and IEEE context
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
- **Reference document sections** by number only (e.g., "See IEEE 1722.1-2021, Section 7.2.1")
- **Implement protocol logic** based on understanding of specifications
- **Create original code** that achieves compliance with standard requirements
- **Use specification constants and values** in implementation (packet types, field sizes, etc.)
- **Reference via MCP-Server** for compliance verification during development only

**LICENSE NOTICE**: These documents are referenced for compliance verification and implementation guidance only. All content remains under copyright of respective standards organizations (IEEE, AES, AVnu Alliance). Any reproduction, distribution, or derivative works require proper licensing from copyright holders.

**Usage Pattern**: When implementing standards-related code, reference these documents via MCP-Server to ensure:
- Protocol message format compliance
- State machine behavior correctness  
- Timing requirement adherence
- Milan professional interoperability
- IEEE specification conformance
- AES audio format compliance
- AVnu interoperability requirements

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
void test_avdecc_entity_descriptor_response(void) {
    avdecc_entity_init(&mock_interface);
    // ... test protocol behavior
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
)

target_link_libraries(standards_protocol_tests 
    aes11_standards 
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
│   ├── AES11/                 # AES11 Digital Audio Reference Signals
│   │   └── 2009/              # AES11-2009 specification (PRIMARY IMPLEMENTATION)
│   │       ├── core/          # Core DARS protocol and state machines
│   │       ├── dars/          # DARS generation and validation
│   │       ├── sync/          # Synchronization methods and timing
│   │       ├── video/         # Video reference synchronization
│   │       └── conformity/    # AES-11 conformance testing
│   ├── AES3/                  # AES3 Digital Audio Interface (foundation)
│   │   └── 2009/              # AES3-2009 specification
│   │       ├── frames/        # AES3 frame format handling
│   │       ├── preambles/     # X/Y/Z preamble detection
│   │       ├── channel/       # Channel status processing
│   │       └── jitter/        # Jitter specifications
│   ├── AES5/                  # AES5 Sampling Frequencies
│   │   └── 2018/              # AES5-2018 specification
│   │       ├── rates/         # Standard sampling frequencies
│   │       └── ratios/        # Multi-rate relationships
│   ├── AES67/                 # AES67 audio-over-IP (integration)
│   │   └── 2018/              # AES67-2018 specification
│   │       └── sync/          # AES67 synchronization with AES-11
│   └── AES70/                 # AES70 device control (OCA)
│       └── 2021/              # AES70-2021 specification
├── SMPTE/                     # Video standards (referenced by AES-11)
│   ├── 318M/                  # Video/audio synchronization
│   │   └── 1999/              # SMPTE 318M-1999
│   └── RP168/                 # Vertical Interval Switching Point
├── IEEE/                      # Optional integration standards
│   └── 1588/                  # PTPv2 (alternative sync method)
│       └── 2019/              # IEEE 1588-2019
├── Common/                    # Cross-standard utilities
│   ├── utils/                 # Shared audio utilities
│   ├── testing/               # Common testing frameworks
│   ├── interfaces/            # Hardware abstraction interfaces
│   └── timing/                # Cross-standard timing utilities
└── Documentation/             # Standards documentation
    ├── conformance/           # Conformance test specifications
    ├── interop/              # Interoperability guidelines
    └── examples/             # Protocol usage examples
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
    namespace AES11 {            // Standard: AES11 (Digital Audio Reference Signals)
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
    
    namespace AES3 {             // Standard: AES3 (Digital Audio Interface foundation)
        namespace _2009 {        // Version: 2009 (year)
            // AES3-2009 digital audio interface implementation
            namespace frames {
                class AES3Frame;
                class SubframeParser;
                class FrameAlignment;
            }
            namespace preambles {
                class PreambleDetector;     // X/Y/Z preamble detection
                class TimingReferenceExtractor;
            }
            namespace channel {
                class ChannelStatusProcessor;
                class UserChannelData;
            }
            namespace jitter {
                class JitterSpecification;  // AES3-4 jitter limits
            }
        }
    }
    
    namespace AES5 {             // Standard: AES5 (Sampling Frequencies)
        namespace _2018 {        // Version: 2018 (year)
            // AES5-2018 sampling frequency standards
            namespace rates {
                class StandardSamplingRates;
                class MultiRateSupport;
            }
            namespace ratios {
                class SampleRateRelationships;
            }
        }
    }
    
    namespace AES67 {            // Standard: AES67 (audio-over-IP integration)
        namespace _2018 {        // Version: 2018 (year)
            namespace sync {
                class AES67DARSIntegration;  // Uses AES11 for sync
            }
        }
    }
    
    namespace AES70 {            // Standard: AES70 (device control)
        namespace _2021 {        // Version: 2021 (year)
            class DeviceControl;
        }
    }
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

// AES Examples (PRIMARY FOCUS):
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

// AES3 Foundation Examples:
lib/Standards/AES/AES3/2009/frames/aes3_frame.hpp                      // AES::AES3::_2009::frames
lib/Standards/AES/AES3/2009/frames/aes3_frame.cpp
lib/Standards/AES/AES3/2009/preambles/preamble_detector.hpp             // AES::AES3::_2009::preambles
lib/Standards/AES/AES3/2009/channel/channel_status_processor.hpp        // AES::AES3::_2009::channel

// AES5 Sampling Examples:
lib/Standards/AES/AES5/2018/rates/standard_sampling_rates.hpp           // AES::AES5::_2018::rates
lib/Standards/AES/AES5/2018/ratios/sample_rate_relationships.hpp        // AES::AES5::_2018::ratios

// SMPTE Video Standards Examples:
lib/Standards/SMPTE/318M/1999/video_audio_synchronization.hpp           // SMPTE::_318M::_1999
lib/Standards/SMPTE/RP168/vertical_interval_switching.hpp               // SMPTE::RP168

// IEEE Examples (Optional integration):
lib/Standards/IEEE/1588/2019/core/ptpv2_protocol.hpp                   // IEEE::_1588::_2019::core

// Common utilities (organization-agnostic):
lib/Standards/Common/interfaces/audio_interface.hpp                     // Common::interfaces
lib/Standards/Common/utils/audio_parser.hpp                            // Common::utils
lib/Standards/Common/testing/test_framework_base.hpp                    // Common::testing
lib/Standards/Common/timing/cross_standard_timing.hpp                   // Common::timing
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

**MANDATORY RULE**: When an IEEE standard references or builds upon another IEEE standard, **ALWAYS reuse the existing implementation** rather than creating redundant code.

#### Examples of Required Cross-Standard Reuse:

**IEEE 1722.1 (AVDECC) Dependencies:**
```cpp
namespace IEEE {
namespace _1722_1 {
namespace _2021 {
namespace aecp {

// ✅ CORRECT - Reuse IEEE 1722 AVTP implementation
#include "../../../1722/2016/avtp/avtp_packet.h"
using IEEE::_1722::_2016::avtp::AVTPPacket;

// ✅ CORRECT - Reuse IEEE 802.1AS time synchronization
#include "../../../802.1/AS/2021/core/time_sync.h"
using IEEE::_802_1::AS::_2021::core::TimeSynchronization;

class AEMCommand {
    // AVDECC commands are transported over AVTP
    IEEE::_1722::_2016::avtp::AVTPPacket create_avtp_packet() {
        // Reuse AVTP implementation, don't reimplement
        return IEEE::_1722::_2016::avtp::AVTPPacket::create_aecp_packet();
    }
    
    // AVDECC requires synchronized time from gPTP
    uint64_t get_synchronized_time() {
        // Reuse gPTP time, don't reimplement time sync
        return IEEE::_802_1::AS::_2021::core::TimeSynchronization::get_current_time();
    }
};

} // namespace aecp
} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
```

**IEEE 1722 (AVTP) Dependencies:**
```cpp
namespace IEEE {
namespace _1722 {
namespace _2016 {
namespace avtp {

// ✅ CORRECT - Reuse IEEE 802.1AS timing for presentation time
#include "../../../802.1/AS/2021/core/time_sync.h"

class StreamDataHeader {
    uint64_t calculate_presentation_time(uint32_t delay_ns) {
        // Reuse gPTP synchronized time, don't reimplement
        auto current_time = IEEE::_802_1::AS::_2021::core::TimeSynchronization::get_current_time();
        return current_time + delay_ns;
    }
};

} // namespace avtp
} // namespace _2016
} // namespace _1722
} // namespace IEEE
```

**Milan Extensions Dependencies:**
```cpp
namespace AVnu {
namespace Milan {
namespace v1_2 {
namespace discovery {

// ✅ CORRECT - Milan builds on IEEE 1722.1, reuse implementation
#include "../../../../IEEE/1722.1/2021/adp/discovery_protocol.h"
#include "../../../../IEEE/1722.1/2021/aem/entity_model.h"

class MilanDiscoveryExtensions : public IEEE::_1722_1::_2021::adp::DiscoveryProtocol {
    // Milan extends IEEE 1722.1 AVDECC, inherit don't reimplement
public:
    // Milan-specific discovery features
    void discover_milan_devices() {
        // Use base IEEE 1722.1 discovery, add Milan extensions
        DiscoveryProtocol::discover_devices();
        apply_milan_filtering();
    }
    
private:
    void apply_milan_filtering() {
        // Milan-specific logic only
    }
};

} // namespace discovery
} // namespace v1_2
} // namespace Milan
} // namespace AVnu
```

#### Forbidden Redundant Implementations:
```cpp
// ❌ WRONG - Reimplementing existing IEEE standards
namespace IEEE {
namespace _1722_1 {
namespace _2021 {

// DON'T DO THIS - AVTP already exists in IEEE::_1722
class AVDECCTransportPacket {  // NO - use IEEE::_1722::_2016::avtp::AVTPPacket
    // ... redundant AVTP implementation
};

// DON'T DO THIS - gPTP already exists in IEEE::_802_1::AS
class AVDECCTimeSync {  // NO - use IEEE::_802_1::AS::_2021::core::TimeSynchronization
    // ... redundant time sync implementation
};

} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
```

#### Cross-Standard Dependency Rules:
1. **IEEE Layering Hierarchy** (higher layers depend on lower layers):
   - **Application Layer**: IEEE 1722.1 (AVDECC)
   - **Transport Layer**: IEEE 1722 (AVTP) 
   - **Timing Layer**: IEEE 802.1AS (gPTP)
   - **Network Layer**: IEEE 802.1Q (VLAN/QoS)

2. **Dependency Direction**: 
   - ✅ **IEEE 1722.1 CAN depend on IEEE 1722 and IEEE 802.1AS**
   - ✅ **IEEE 1722 CAN depend on IEEE 802.1AS**
   - ❌ **IEEE 802.1AS CANNOT depend on IEEE 1722 or IEEE 1722.1**

3. **Extension Standards**:
   - ✅ **Milan CAN depend on any IEEE standard it extends**
   - ✅ **AES67 CAN depend on IEEE standards it references**
   - ✅ **Multiple standard versions CAN coexist** (2021, 2016, 2013)

#### CMake Dependencies for Cross-Standard Reuse:
```cmake
# ✅ CORRECT - Respect IEEE layering in CMake dependencies
target_link_libraries(ieee_1722_1_2021
    ieee_1722_2016               # AVDECC depends on AVTP
    ieee_802_1_as_2021          # AVDECC depends on gPTP
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
# target_link_libraries(ieee_802_1_as_2021
#     ieee_1722_2016            # NO - gPTP cannot depend on AVTP
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
namespace IEEE {
namespace intel {              // NO - hardware vendor in IEEE namespace
    class IntelAVBInterface;
}
}

// ❌ WRONG - OS-specific namespaces in IEEE standards
namespace IEEE {
namespace windows {            // NO - OS specific in IEEE namespace
    class WinSockInterface;
}
}

// ❌ WRONG - implementation details in IEEE namespace  
namespace IEEE {
namespace _1722_1 {
namespace _2021 {
    class WindowsSocketImpl;  // NO - implementation detail, not protocol
}
}
}

// ✅ CORRECT - IEEE standards are pure protocol implementations
namespace IEEE {
namespace _1722_1 {
namespace _2021 {
namespace aem {
    class EntityModel;        // YES - pure IEEE 1722.1-2021 protocol
}
}
}
}
```

### CMake Integration with Correct Structure
```cmake
# ✅ CORRECT CMake structure following AES-11 hierarchy
add_library(aes11_2009 STATIC
    AES/AES11/2009/core/dars_protocol.cpp
    AES/AES11/2009/core/synchronization_manager.cpp
    AES/AES11/2009/dars/dars_generator.cpp
    AES/AES11/2009/dars/dars_validator.cpp
    AES/AES11/2009/sync/video_reference_sync.cpp
    AES/AES11/2009/video/video_sync_alignment.cpp
    AES/AES11/2009/conformity/aes11_conformance_test.cpp
)

add_library(aes3_2009 STATIC
    AES/AES3/2009/frames/aes3_frame.cpp
    AES/AES3/2009/preambles/preamble_detector.cpp
    AES/AES3/2009/channel/channel_status_processor.cpp
)

add_library(aes5_2018 STATIC
    AES/AES5/2018/rates/standard_sampling_rates.cpp
    AES/AES5/2018/ratios/sample_rate_relationships.cpp
)

add_library(smpte_video STATIC
    SMPTE/318M/1999/video_audio_synchronization.cpp
    SMPTE/RP168/vertical_interval_switching.cpp
)

add_library(standards_common STATIC
    Common/interfaces/audio_interface.cpp
    Common/utils/audio_parser.cpp
    Common/testing/test_framework_base.cpp
    Common/timing/cross_standard_timing.cpp
)

# AES standards libraries dependencies
target_link_libraries(aes11_2009
    aes3_2009                   # AES-11 depends on AES3 frame format
    aes5_2018                   # AES-11 depends on AES5 sampling rates
    smpte_video                 # AES-11 supports video synchronization
    standards_common            # All can use Common utilities
)
```

### Documentation Structure Requirements - Corrected
```cpp
/**
 * @file conformity_test_framework.h
 * @brief IEEE 802.1AS-2021 Conformity Testing Framework
 * @namespace IEEE::_802_1::AS::_2021::Testing
 * 
 * Implements conformity testing according to IEEE 802.1AS-2021 specification.
 * This namespace contains all testing functionality for validating IEEE 802.1AS-2021
 * compliance including state machine behavior, timing requirements, and interoperability.
 * 
 * @see IEEE 802.1AS-2021, Clauses 11.2-11.5 "Conformance requirements"
 * @see IEEE 802.1AS-2021, Annex A "Implementation conformance statement (ICS)"
 */
```

### Enforcement Rules - Corrected with Copyright Compliance
1. **IEEE namespaces are top-level** - not wrapped in `openavnu::standards`
2. **Namespace must match folder structure** exactly (`IEEE::_802_1::AS::_2021` = `IEEE/802.1/AS/2021/`)
3. **Version numbers use underscores** in namespaces (`_2021`, `_2016`) to avoid conflicts
4. **Dots become underscores** in namespaces (`802.1` becomes `_802_1`, `1722.1` becomes `_1722_1`)
5. **No hardware vendors** in IEEE namespace hierarchy
6. **No OS-specific namespaces** in IEEE standards
7. **IEEE layering respected** - higher layer standards can depend on lower layers
8. **Cross-standard utilities** only in Common namespace
9. **Conformance testing** isolated in Testing sub-namespaces
10. **COPYRIGHT COMPLIANCE MANDATORY**:
    - **NEVER reproduce copyrighted specification content** in source code or documentation
    - **Reference specifications by section number only** (e.g., "IEEE 1722.1-2021, Section 7.2.1")
    - **Implement based on understanding**, not by copying specification text
    - **Use MCP-Server for compliance verification only**, not content reproduction
    - **Respect all copyright holders**: IEEE, AES, AVnu Alliance, ITU, etc.
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

This structure ensures clear separation of IEEE standards versions, prevents architectural violations, and maintains the hardware-agnostic principle while following the actual implementation pattern used in the codebase.

This architecture ensures the Standards layer remains pure, testable, reusable across different hardware platforms, and maintains strict IEEE compliance while following OpenAvnu's core development principles.

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