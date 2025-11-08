#include "aes3_adapter.hpp"

#include <cstddef>

#ifdef AES3_INTEGRATION
// Submodule layout provides headers in implementation src; include SubframeBuilder constants.
#include "AES/AES3/Part3/_2009/subframe/subframe_builder.hpp"
#endif

namespace AES {
namespace AES11 {
namespace _2009 {
namespace core {

// Helper: read 8-bit window starting at bitOffset from a bitstream buffer of given bitLength.
static inline uint8_t read_u8_at_bit_offset(const uint8_t* buf, size_t bitLength, size_t bitOffset) noexcept {
    const size_t totalBytes = (bitLength + 7u) / 8u;
    const size_t byteIndex = bitOffset / 8u;
    const uint8_t bitInByte = static_cast<uint8_t>(bitOffset & 0x7u);
    uint16_t two = 0;
    if (byteIndex < totalBytes) {
        two |= buf[byteIndex];
    }
    if ((byteIndex + 1u) < totalBytes) {
        two |= static_cast<uint16_t>(buf[byteIndex + 1u]) << 8;
    }
    return static_cast<uint8_t>((two >> bitInByte) & 0xFFu);
}

static inline bool is_preamble_byte(uint8_t b) noexcept {
#ifdef AES3_INTEGRATION
    return (b == AES::AES3::Part3::_2009::subframe::SubframeBuilder::PREAMBLE_X_PATTERN)
        || (b == AES::AES3::Part3::_2009::subframe::SubframeBuilder::PREAMBLE_Y_PATTERN)
        || (b == AES::AES3::Part3::_2009::subframe::SubframeBuilder::PREAMBLE_Z_PATTERN);
#else
    return (b == 0xE2u) || (b == 0xE4u) || (b == 0xE8u);
#endif
}

static inline TRPInfo::PreambleType to_preamble_type(uint8_t b) noexcept {
#ifdef AES3_INTEGRATION
    if (b == AES::AES3::Part3::_2009::subframe::SubframeBuilder::PREAMBLE_X_PATTERN) return TRPInfo::PreambleType::X;
    if (b == AES::AES3::Part3::_2009::subframe::SubframeBuilder::PREAMBLE_Y_PATTERN) return TRPInfo::PreambleType::Y;
    if (b == AES::AES3::Part3::_2009::subframe::SubframeBuilder::PREAMBLE_Z_PATTERN) return TRPInfo::PreambleType::Z;
#else
    if (b == 0xE2u) return TRPInfo::PreambleType::X;
    if (b == 0xE4u) return TRPInfo::PreambleType::Y;
    if (b == 0xE8u) return TRPInfo::PreambleType::Z;
#endif
    return TRPInfo::PreambleType::Unknown;
}

// Detect preamble from first preamble byte (LSB-first representation of time slots 0-3)
bool AES3Adapter::detect_preamble(const uint8_t* frameBytes, size_t bitLength) noexcept {
    if (!frameBytes || bitLength < 8) return false;
    // Scan bit windows for any preamble match within provided bit range
    const size_t maxStart = bitLength - 8u;
    for (size_t off = 0; off <= maxStart; ++off) {
        uint8_t w = read_u8_at_bit_offset(frameBytes, bitLength, off);
        if (is_preamble_byte(w)) {
            return true;
        }
    }
    return false;
}

TRPInfo AES3Adapter::extract_trp(const uint8_t* frameBytes, size_t bitLength) noexcept {
    TRPInfo info{};
    if (!frameBytes || bitLength < 8) return info;

    const size_t maxStart = bitLength - 8u;
    for (size_t off = 0; off <= maxStart; ++off) {
        uint8_t w = read_u8_at_bit_offset(frameBytes, bitLength, off);
        if (is_preamble_byte(w)) {
            info.type = to_preamble_type(w);
            info.valid = (info.type != TRPInfo::PreambleType::Unknown);
            if (info.valid) {
                info.preamble_offset_bits = off;
            }
            break;
        }
    }
    return info;
}

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES
