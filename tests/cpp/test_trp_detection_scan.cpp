#include <gtest/gtest.h>
#include <array>
#include <cstdint>
#include "../../lib/Standards/AES/AES11/2009/core/aes3_adapter.hpp"

using AES::AES11::_2009::core::AES3Adapter;
using AES::AES11::_2009::core::TRPInfo;

static void place_pattern_with_offset(uint8_t pattern, size_t bit_offset, std::array<uint8_t,2>& buf) {
    // Zero initialize and then insert 8-bit pattern starting at bit_offset within 16-bit buffer
    buf[0] = 0; buf[1] = 0;
    // Construct 16-bit container
    uint16_t window = static_cast<uint16_t>(pattern) << bit_offset;
    buf[0] = static_cast<uint8_t>(window & 0xFF);
    buf[1] = static_cast<uint8_t>((window >> 8) & 0xFF);
}

TEST(AES3TRPScanTests, DetectsPreambleAcrossBitOffsetsAndReportsOffset) {
    // X/Y/Z patterns
    const uint8_t patterns[3] = { 0xE2, 0xE4, 0xE8 };
    for (uint8_t pat : patterns) {
        for (size_t shift = 0; shift < 8; ++shift) {
            std::array<uint8_t,2> buf{};
            place_pattern_with_offset(pat, shift, buf);
            ASSERT_TRUE(AES3Adapter::detect_preamble(buf.data(), 16)) << "pattern=" << std::hex << (int)pat << " shift=" << std::dec << shift;
            TRPInfo info = AES3Adapter::extract_trp(buf.data(), 16);
            EXPECT_TRUE(info.valid);
            // type must match pattern used
            EXPECT_EQ(static_cast<uint8_t>(info.type), pat);
            ASSERT_TRUE(info.preamble_offset_bits.has_value());
            EXPECT_EQ(info.preamble_offset_bits.value(), shift) << "pattern=" << std::hex << (int)pat;
        }
    }
}

#ifdef AES3_INTEGRATION
#include "AES/AES3/Part3/_2009/subframe/subframe_builder.hpp"
#include "AES/AES3/Part3/_2009/subframe/subframe_data.hpp"
TEST(AES3TRPScanTests, Integration_GeneratedSubframeRecognized) {
    using AES::AES3::Part3::_2009::subframe::SubframeBuilder;
    using AES::AES3::Part3::_2009::subframe::SubframeData;
    SubframeBuilder builder;
    SubframeData sf;
    // Build a subframe with preamble Z and zero audio
    ASSERT_EQ(0, builder.build_subframe(0, 0, 0, 0, SubframeBuilder::Preamble::Z, sf));
    // The first byte of SubframeData (slots 0..3 -> 8 bits) must equal PREAMBLE_Z_PATTERN
    uint8_t preamble_byte = static_cast<uint8_t>(sf.get_bit(0) | (sf.get_bit(1) << 2) | (sf.get_bit(2) << 4) | (sf.get_bit(3) << 6));
    std::array<uint8_t,2> buf{preamble_byte, 0};
    auto info = AES3Adapter::extract_trp(buf.data(), 16);
    EXPECT_TRUE(info.valid);
    EXPECT_EQ(static_cast<uint8_t>(info.type), SubframeBuilder::PREAMBLE_Z_PATTERN);
    ASSERT_TRUE(info.preamble_offset_bits.has_value());
    EXPECT_EQ(info.preamble_offset_bits.value(), 0u);
}
#endif
