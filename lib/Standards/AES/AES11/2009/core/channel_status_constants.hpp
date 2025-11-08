// Traceability Header
// Module: lib/Standards/AES/AES11/2009/core/channel_status_constants.hpp
// Phase: 05-implementation
// Traceability:
//   Design: DES-C-001 (Channel Status Mapping Utilities)
//   Requirements: REQ-F-CS-ANNEXA-FLAGS, REQ-F-CS-ANNEXA-LEN, REQ-F-CS-ANNEXA-DT, REQ-F-CS-ANNEXA-TZ
//   Tests: TEST-UNIT-ChannelStatusConstants, ChannelStatusAnnexATests.*
// Notes: Centralizes indices and masks to prevent drift/overlap (SFMEA FM-CS-002/FM-CS-009)

#ifndef AES_AES11_2009_CORE_CHANNEL_STATUS_CONSTANTS_HPP
#define AES_AES11_2009_CORE_CHANNEL_STATUS_CONSTANTS_HPP

#include <cstdint>
#include <cstddef>

namespace AES {
namespace AES11 {
namespace _2009 {
namespace core {
namespace constants {

// AES3 channel status byte 4 (zero-based index 3) — holds DARS grade bits 0..1
static constexpr size_t kByte4Index = 3; // "byte 4" -> index 3

// UTC flags location (implementation-defined within our mapping; see design delta)
static constexpr size_t kUtcFlagsIndex = 17;        // index for UTC/DST/other flags
static constexpr uint8_t kUtcValidMask = 0x01;      // bit 0
static constexpr uint8_t kLeapPendingMask = 0x02;   // bit 1
static constexpr uint8_t kDstMask = 0x04;           // bit 2 (internal)
static constexpr uint8_t kNonAudioMask = 0x08;      // bit 3 (internal)
static constexpr uint8_t kAlignmentMarkerMask = 0x10;// bit 4 (internal)

// Timezone offset (signed 16-bit, minutes, little-endian)
static constexpr size_t kTzOffsetLoIndex = 18;      // low byte
static constexpr size_t kTzOffsetHiIndex = 19;      // high byte

// Date/time compact mapping (YY MM DD HH MM SS) contiguous region
static constexpr size_t kDateTimeStartIndex = 11;   // inclusive
static constexpr size_t kDateTimeEndIndex   = 16;   // inclusive

// Bounds for a single channel status block (we operate on 24 bytes in tests)
static constexpr size_t kChannelStatusMinBytes = 24;

} // namespace constants
} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES

#endif // AES_AES11_2009_CORE_CHANNEL_STATUS_CONSTANTS_HPP
