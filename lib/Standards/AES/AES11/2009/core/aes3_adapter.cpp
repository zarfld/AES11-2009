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

// Detect preamble from first preamble byte (LSB-first representation of time slots 0-3)
bool AES3Adapter::detect_preamble(const uint8_t* frameBytes, size_t bitLength) noexcept {
    if (!frameBytes || bitLength < 8) return false;

    const uint8_t preamble_byte = frameBytes[0];

#ifdef AES3_INTEGRATION
    // Use patterns from subframe builder constants
    return (preamble_byte == AES::AES3::Part3::_2009::subframe::SubframeBuilder::PREAMBLE_X_PATTERN)
        || (preamble_byte == AES::AES3::Part3::_2009::subframe::SubframeBuilder::PREAMBLE_Y_PATTERN)
        || (preamble_byte == AES::AES3::Part3::_2009::subframe::SubframeBuilder::PREAMBLE_Z_PATTERN);
#else
    return (preamble_byte == 0xE2u) || (preamble_byte == 0xE4u) || (preamble_byte == 0xE8u);
#endif
}

TRPInfo AES3Adapter::extract_trp(const uint8_t* frameBytes, size_t bitLength) noexcept {
    TRPInfo info{};
    if (!frameBytes || bitLength < 8) return info;

    const uint8_t preamble_byte = frameBytes[0];
    if (!detect_preamble(frameBytes, bitLength)) return info;

#ifdef AES3_INTEGRATION
    if (preamble_byte == AES::AES3::Part3::_2009::subframe::SubframeBuilder::PREAMBLE_X_PATTERN)
        info.type = TRPInfo::PreambleType::X;
    else if (preamble_byte == AES::AES3::Part3::_2009::subframe::SubframeBuilder::PREAMBLE_Y_PATTERN)
        info.type = TRPInfo::PreambleType::Y;
    else if (preamble_byte == AES::AES3::Part3::_2009::subframe::SubframeBuilder::PREAMBLE_Z_PATTERN)
        info.type = TRPInfo::PreambleType::Z;
#else
    if (preamble_byte == 0xE2u) info.type = TRPInfo::PreambleType::X;
    else if (preamble_byte == 0xE4u) info.type = TRPInfo::PreambleType::Y;
    else if (preamble_byte == 0xE8u) info.type = TRPInfo::PreambleType::Z;
#endif

    info.valid = (info.type != TRPInfo::PreambleType::Unknown);

    // For TRP offset, treat the preamble start as bit offset 0 within the provided buffer.
    // In richer pipelines, this can be derived from the exact X/Z transition position.
    if (info.valid) {
        info.preamble_offset_bits = static_cast<size_t>(0);
    }
    return info;
}

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES
