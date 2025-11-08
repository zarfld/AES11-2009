#include <gtest/gtest.h>
#include "../../lib/Standards/AES/AES11/2009/core/aes3_adapter.hpp"

using AES::AES11::_2009::core::AES3Adapter;
using AES::AES11::_2009::core::TRPInfo;

TEST(AES3AdapterTests, DetectPreambleRecognizesValidPatterns) {
    uint8_t x[1] = {0xE2};
    uint8_t y[1] = {0xE4};
    uint8_t z[1] = {0xE8};
    EXPECT_TRUE(AES3Adapter::detect_preamble(x, 8));
    EXPECT_TRUE(AES3Adapter::detect_preamble(y, 8));
    EXPECT_TRUE(AES3Adapter::detect_preamble(z, 8));
}

TEST(AES3AdapterTests, DetectPreambleRejectsInvalidPatterns) {
    uint8_t zeros[1] = {0x00};
    uint8_t ff[1] = {0xFF};
    uint8_t near[1] = {0xE0}; // Similar but invalid
    EXPECT_FALSE(AES3Adapter::detect_preamble(zeros, 8));
    EXPECT_FALSE(AES3Adapter::detect_preamble(ff, 8));
    EXPECT_FALSE(AES3Adapter::detect_preamble(near, 8));
}

TEST(AES3AdapterTests, ExtractTrpValidForRecognizedPreamble) {
    uint8_t frame_x[1] = {0xE2};
    TRPInfo info_x = AES3Adapter::extract_trp(frame_x, 8);
    EXPECT_TRUE(info_x.valid);
    EXPECT_EQ(static_cast<uint8_t>(info_x.type), 0xE2);
    ASSERT_TRUE(info_x.preamble_offset_bits.has_value());
    EXPECT_EQ(info_x.preamble_offset_bits.value(), 0u);

    uint8_t frame_z[1] = {0xE8};
    TRPInfo info_z = AES3Adapter::extract_trp(frame_z, 8);
    EXPECT_TRUE(info_z.valid);
    EXPECT_EQ(static_cast<uint8_t>(info_z.type), 0xE8);
}

TEST(AES3AdapterTests, ExtractTrpInvalidWhenNoPreamble) {
    uint8_t invalid[1] = {0xE0};
    TRPInfo info = AES3Adapter::extract_trp(invalid, 8);
    EXPECT_FALSE(info.valid);
    EXPECT_EQ(static_cast<uint8_t>(info.type), 0x00);
    EXPECT_FALSE(info.preamble_offset_bits.has_value());
}
