#include "channel_status_utils.hpp"

#include <cstring>

namespace AES {
namespace AES11 {
namespace _2009 {
namespace core {

static constexpr size_t CS_BYTE4_INDEX = 3; // zero-based: byte 4 is index 3

DARSGrade ChannelStatusUtils::extract_grade(const uint8_t* channelStatus, size_t length) {
    if (!channelStatus || length <= CS_BYTE4_INDEX) return DARSGrade::Unknown;
    uint8_t b4 = channelStatus[CS_BYTE4_INDEX];
    uint8_t bits = static_cast<uint8_t>(b4 & 0x03u);
    switch (bits) {
        case 0x01: return DARSGrade::Grade1;
        case 0x02: return DARSGrade::Grade2;
        case 0x03: return DARSGrade::Reserved;
        default:   return DARSGrade::Unknown;
    }
}

bool ChannelStatusUtils::set_grade(uint8_t* channelStatus, size_t length, DARSGrade grade) {
    if (!channelStatus || length <= CS_BYTE4_INDEX) return false;
    uint8_t b4 = channelStatus[CS_BYTE4_INDEX];
    b4 &= static_cast<uint8_t>(~0x03u); // clear bits 0-1
    uint8_t val = 0x00;
    switch (grade) {
        case DARSGrade::Grade1: val = 0x01; break;
        case DARSGrade::Grade2: val = 0x02; break;
        case DARSGrade::Reserved: val = 0x03; break;
        case DARSGrade::Unknown: default: val = 0x00; break;
    }
    channelStatus[CS_BYTE4_INDEX] = static_cast<uint8_t>(b4 | val);
    return true;
}

static inline bool in_range(uint8_t v, uint8_t lo, uint8_t hi) { return v >= lo && v <= hi; }

bool ChannelStatusUtils::encode_datetime(const DateTimeFields& dt, uint8_t* out6) {
    if (!out6) return false;
    if (!in_range(dt.year, 0, 99) || !in_range(dt.month, 1, 12) || !in_range(dt.day, 1, 31) ||
        !in_range(dt.hour, 0, 23) || !in_range(dt.minute, 0, 59) || !in_range(dt.second, 0, 59)) {
        return false;
    }
    // Minimal compact layout (implementation-defined scaffolding):
    // [0]=year, [1]=month, [2]=day, [3]=hour, [4]=minute, [5]=second with flags in MSBs
    // Place flags (utc<<7 | leap<<6) into seconds' upper bits without exceeding 59 range.
    // To avoid modifying second value range, we store flags implicitly as out-of-band during integration.
    // For now, keep seconds verbatim; flags not encoded into compact form.
    out6[0] = dt.year;
    out6[1] = dt.month;
    out6[2] = dt.day;
    out6[3] = dt.hour;
    out6[4] = dt.minute;
    out6[5] = dt.second;
    (void)dt.utc; (void)dt.leapSecond; // flags reserved for expanded mapping during full integration
    return true;
}

std::optional<DateTimeFields> ChannelStatusUtils::decode_datetime(const uint8_t* in6) {
    if (!in6) return std::nullopt;
    DateTimeFields dt { in6[0], in6[1], in6[2], in6[3], in6[4], in6[5], false, false };
    if (!in_range(dt.month, 1, 12) || !in_range(dt.day, 1, 31) || !in_range(dt.hour, 0, 23) ||
        !in_range(dt.minute, 0, 59) || !in_range(dt.second, 0, 59)) {
        return std::nullopt;
    }
    return dt;
}

bool ChannelStatusUtils::set_masked_bits(uint8_t* buffer, size_t length, size_t byteIndex, uint8_t mask, uint8_t valueMasked) {
    if (!buffer || byteIndex >= length) return false;
    // Ensure valueMasked only occupies bits within mask
    if ((valueMasked & ~mask) != 0) return false;
    uint8_t current = buffer[byteIndex];
    current &= static_cast<uint8_t>(~mask); // clear masked bits
    current |= valueMasked;                 // set new masked bits (already aligned by caller)
    buffer[byteIndex] = current;
    return true;
}

std::optional<uint8_t> ChannelStatusUtils::read_masked_bits(const uint8_t* buffer, size_t length, size_t byteIndex, uint8_t mask, uint8_t shift) {
    if (!buffer || byteIndex >= length) return std::nullopt;
    uint8_t current = buffer[byteIndex];
    uint8_t masked = static_cast<uint8_t>(current & mask);
    if (shift > 0) masked = static_cast<uint8_t>(masked >> shift);
    return masked;
}

std::optional<bool> ChannelStatusUtils::read_flag(const uint8_t* buffer, size_t length, size_t byteIndex, uint8_t mask) {
    auto val = read_masked_bits(buffer, length, byteIndex, mask, 0);
    if (!val.has_value()) return std::nullopt;
    return (*val) != 0;
}

bool ChannelStatusUtils::write_flag(uint8_t* buffer, size_t length, size_t byteIndex, uint8_t mask, bool enabled) {
    if (!buffer || byteIndex >= length) return false;
    uint8_t current = buffer[byteIndex];
    if (enabled) {
        current = static_cast<uint8_t>(current | mask);
    } else {
        current = static_cast<uint8_t>(current & ~mask);
    }
    buffer[byteIndex] = current;
    return true;
}

// --- Annex A UTC / Timezone mapping ---
// Assumed byte positions within channel status block (see header notes):
static constexpr size_t CS_UTC_FLAGS_INDEX = 17; // byte 18 (zero-based index 17)
static constexpr uint8_t CS_UTC_VALID_MASK = 0x01; // bit 0
static constexpr uint8_t CS_LEAP_PENDING_MASK = 0x02; // bit 1
static constexpr uint8_t CS_DST_MASK = 0x04; // bit 2 (implementation-defined DST flag)
static constexpr size_t CS_TZ_OFFSET_LO_INDEX = 18; // byte 19 low
static constexpr size_t CS_TZ_OFFSET_HI_INDEX = 19; // byte 20 high
// Date/time mapping start (implementation-defined, contiguous 6 bytes for YY MM DD HH MM SS)
// Revised date/time mapping within 24-byte channel status block (indices 0..23).
// Use bytes 12..17 (indices 11..16) for YY MM DD HH MM SS to avoid collision with:
//  - byte 17 (UTC flags) already used
//  - bytes 18-19 (timezone offset)
static constexpr size_t CS_DT_START_INDEX = 11; // byte 12
static constexpr size_t CS_DT_END_INDEX   = 16; // inclusive (byte 17 is flags)

static inline bool tz_offset_in_range(int32_t minutes) {
    // Common practical bounds: UTC-12:00 (-720) .. UTC+14:00 (+840)
    return minutes >= -720 && minutes <= 840;
}

std::optional<UTCInfo> ChannelStatusUtils::extract_utc_info(const uint8_t* channelStatus, size_t length) {
    if (!channelStatus) return std::nullopt;
    if (length <= CS_TZ_OFFSET_HI_INDEX) return std::nullopt;
    UTCInfo info{};
    auto utcValidOpt = read_flag(channelStatus, length, CS_UTC_FLAGS_INDEX, CS_UTC_VALID_MASK);
    auto leapOpt = read_flag(channelStatus, length, CS_UTC_FLAGS_INDEX, CS_LEAP_PENDING_MASK);
    if (!utcValidOpt.has_value() || !leapOpt.has_value()) return std::nullopt;
    info.utcValid = *utcValidOpt;
    info.leapSecondPending = *leapOpt;
    // Little-endian signed 16-bit minutes offset
    uint16_t lo = channelStatus[CS_TZ_OFFSET_LO_INDEX];
    uint16_t hi = channelStatus[CS_TZ_OFFSET_HI_INDEX];
    int16_t signedVal = static_cast<int16_t>((hi << 8) | lo);
    info.timezoneOffsetMinutes = signedVal;
    if (!tz_offset_in_range(info.timezoneOffsetMinutes)) {
        return std::nullopt;
    }
    return info;
}

bool ChannelStatusUtils::set_utc_info(uint8_t* channelStatus, size_t length, const UTCInfo& info) {
    if (!channelStatus) return false;
    if (length <= CS_TZ_OFFSET_HI_INDEX) return false;
    if (!tz_offset_in_range(info.timezoneOffsetMinutes)) return false;
    // Write flags
    if (!write_flag(channelStatus, length, CS_UTC_FLAGS_INDEX, CS_UTC_VALID_MASK, info.utcValid)) return false;
    if (!write_flag(channelStatus, length, CS_UTC_FLAGS_INDEX, CS_LEAP_PENDING_MASK, info.leapSecondPending)) return false;
    // Write little-endian signed 16-bit minutes offset
    uint16_t raw = static_cast<uint16_t>(static_cast<uint16_t>(info.timezoneOffsetMinutes) & 0xFFFFu);
    channelStatus[CS_TZ_OFFSET_LO_INDEX] = static_cast<uint8_t>(raw & 0xFFu);
    channelStatus[CS_TZ_OFFSET_HI_INDEX] = static_cast<uint8_t>((raw >> 8) & 0xFFu);
    return true;
}

static inline bool dt_fields_in_range(const DateTimeFields& dt) {
    if (dt.month < 1 || dt.month > 12) return false;
    if (dt.day < 1 || dt.day > 31) return false;
    if (dt.hour > 23) return false;
    if (dt.minute > 59) return false;
    // Allow second==59 always; disallow 60 (we represent leap via flag on second==59)
    if (dt.second > 59) return false;
    if (dt.year > 99) return false;
    // If leapSecond flag is true, we expect second==59 to represent the leap boundary (no 60 stored)
    if (dt.leapSecond && dt.second != 59) return false;
    return true;
}

bool ChannelStatusUtils::set_datetime_info(uint8_t* channelStatus, size_t length, const DateTimeFields& dt) {
    if (!channelStatus) return false;
    if (length <= CS_DT_END_INDEX) return false; // need at least 17+1 bytes (index 16 accessible)
    if (!dt_fields_in_range(dt)) return false;
    // Write UTC flag and leap-second into UTC flags byte, reuse existing helpers
    if (!write_flag(channelStatus, length, CS_UTC_FLAGS_INDEX, CS_UTC_VALID_MASK, dt.utc)) return false;
    if (!write_flag(channelStatus, length, CS_UTC_FLAGS_INDEX, CS_LEAP_PENDING_MASK, dt.leapSecond)) return false;
    // Write compact YY MM DD HH MM SS
    channelStatus[CS_DT_START_INDEX + 0] = dt.year;
    channelStatus[CS_DT_START_INDEX + 1] = dt.month;
    channelStatus[CS_DT_START_INDEX + 2] = dt.day;
    channelStatus[CS_DT_START_INDEX + 3] = dt.hour;
    channelStatus[CS_DT_START_INDEX + 4] = dt.minute;
    channelStatus[CS_DT_START_INDEX + 5] = dt.second;
    return true;
}

std::optional<DateTimeFields> ChannelStatusUtils::extract_datetime_info(const uint8_t* channelStatus, size_t length) {
    if (!channelStatus) return std::nullopt;
    if (length <= CS_DT_END_INDEX) return std::nullopt;
    DateTimeFields dt{};
    dt.year   = channelStatus[CS_DT_START_INDEX + 0];
    dt.month  = channelStatus[CS_DT_START_INDEX + 1];
    dt.day    = channelStatus[CS_DT_START_INDEX + 2];
    dt.hour   = channelStatus[CS_DT_START_INDEX + 3];
    dt.minute = channelStatus[CS_DT_START_INDEX + 4];
    dt.second = channelStatus[CS_DT_START_INDEX + 5];
    auto utcFlag = read_flag(channelStatus, length, CS_UTC_FLAGS_INDEX, CS_UTC_VALID_MASK);
    auto leapFlag = read_flag(channelStatus, length, CS_UTC_FLAGS_INDEX, CS_LEAP_PENDING_MASK);
    if (!utcFlag.has_value() || !leapFlag.has_value()) return std::nullopt;
    dt.utc = *utcFlag;
    dt.leapSecond = *leapFlag;
    if (!dt_fields_in_range(dt)) return std::nullopt;
    return dt;
}

std::optional<bool> ChannelStatusUtils::read_dst_flag(const uint8_t* channelStatus, size_t length) {
    return read_flag(channelStatus, length, CS_UTC_FLAGS_INDEX, CS_DST_MASK);
}

bool ChannelStatusUtils::set_dst_flag(uint8_t* channelStatus, size_t length, bool enabled) {
    return write_flag(channelStatus, length, CS_UTC_FLAGS_INDEX, CS_DST_MASK, enabled);
}

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES
