// Channel Status Utilities - DES-C-001 extension
// Provides grade identification and date/time encode/decode helpers for AES3 channel status
// Original implementation based on understanding of AES-11-2009 (Sections 5.1.3, Annex A) and AES3.
// No copyrighted specification text reproduced.

#ifndef AES_AES11_2009_CORE_CHANNEL_STATUS_UTILS_HPP
#define AES_AES11_2009_CORE_CHANNEL_STATUS_UTILS_HPP

#include <cstdint>
#include <array>
#include <ctime>
#include <optional>

namespace AES {
namespace AES11 {
namespace _2009 {
namespace core {

// Grade bits (channel status byte 4 bits 0-1) per AES-11 Section 5.1.3
enum class DARSGrade : uint8_t {
    Unknown = 0, // 00 default / not explicitly set
    Grade1  = 1, // 01
    Grade2  = 2, // 10
    Reserved = 3 // 11 reserved
};

struct DateTimeFields {
    uint8_t year;   // 0-99 (add base e.g. 2000 externally)
    uint8_t month;  // 1-12
    uint8_t day;    // 1-31
    uint8_t hour;   // 0-23
    uint8_t minute; // 0-59
    uint8_t second; // 0-59 (omit leap second explicit 60 in encode)
    bool utc;       // true if UTC
    bool leapSecond;// indicates leap second event boundary
};

class ChannelStatusUtils {
public:
    // Extract grade from channel status byte 4 (bits 0-1). Input: 192-byte channel status block (AES3 stereo frame block)
    static DARSGrade extract_grade(const uint8_t* channelStatus, size_t length);

    // Set grade bits in channel status byte 4 preserving other bits.
    static bool set_grade(uint8_t* channelStatus, size_t length, DARSGrade grade);

    // Encode date/time fields into a 6-byte compact structure (not full AES3 Annex A format)
    // Returns false if fields out of range. This scaffolding intentionally minimal.
    static bool encode_datetime(const DateTimeFields& dt, uint8_t* out6);

    // Decode date/time fields from 6-byte compact structure; returns std::nullopt if invalid.
    static std::optional<DateTimeFields> decode_datetime(const uint8_t* in6);

    // Generic bit/field helpers (scaffolding) to support parameterized channel status mappings
    // without hardcoding AES3/AES-11 bit positions in this layer.

    // Set or clear masked bits in a given byte index. Preserves other bits.
    static bool set_masked_bits(uint8_t* buffer, size_t length, size_t byteIndex, uint8_t mask, uint8_t valueMasked);

    // Read masked bits (right-shifted by shift) from a given byte index.
    static std::optional<uint8_t> read_masked_bits(const uint8_t* buffer, size_t length, size_t byteIndex, uint8_t mask, uint8_t shift = 0);

    // Convenience for boolean flags stored in a mask (value is true if any masked bit is 1).
    static std::optional<bool> read_flag(const uint8_t* buffer, size_t length, size_t byteIndex, uint8_t mask);

    static bool write_flag(uint8_t* buffer, size_t length, size_t byteIndex, uint8_t mask, bool enabled);
};

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES

#endif // AES_AES11_2009_CORE_CHANNEL_STATUS_UTILS_HPP
