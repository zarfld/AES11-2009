#include <gtest/gtest.h>
#include "../../lib/Standards/AES/AES11/2009/core/channel_status_utils.hpp"

using AES::AES11::_2009::core::ChannelStatusUtils;

TEST(ChannelStatusBitfieldTests, WriteAndReadMaskedBitsPreserveOthers) {
    uint8_t cs[8] = {0};
    // Preload neighboring bits to verify preservation
    cs[2] = 0b1010'1010; // byte index 2

    // Target mask 0b00011100 at byte 2, value 0b00010100 (shifted already to mask position)
    uint8_t mask = 0b00011100;
    uint8_t valueMasked = 0b00010100;
    ASSERT_TRUE(ChannelStatusUtils::set_masked_bits(cs, sizeof(cs), 2, mask, valueMasked));

    // Check preservation of unmasked bits
    // original unmasked bits: b7,b6,b5,b1,b0 from 0b10101010 -> b7=1,b6=0,b5=1,b1=1,b0=0
    EXPECT_EQ(cs[2] & 0b11100011, 0b10100010);

    // Read back masked field without shift
    auto raw = ChannelStatusUtils::read_masked_bits(cs, sizeof(cs), 2, mask, 0);
    ASSERT_TRUE(raw.has_value());
    EXPECT_EQ(*raw, valueMasked);

    // Read back with shift of 2 to normalize
    auto norm = ChannelStatusUtils::read_masked_bits(cs, sizeof(cs), 2, mask, 2);
    ASSERT_TRUE(norm.has_value());
    EXPECT_EQ(*norm, static_cast<uint8_t>(valueMasked >> 2));
}

TEST(ChannelStatusBitfieldTests, WriteAndReadFlag) {
    uint8_t cs[4] = {0};
    // Use byte index 1, mask 0b00100000
    ASSERT_TRUE(ChannelStatusUtils::write_flag(cs, sizeof(cs), 1, 0b00100000, true));
    auto f1 = ChannelStatusUtils::read_flag(cs, sizeof(cs), 1, 0b00100000);
    ASSERT_TRUE(f1.has_value());
    EXPECT_TRUE(*f1);

    // Clearing should reset only that bit
    ASSERT_TRUE(ChannelStatusUtils::write_flag(cs, sizeof(cs), 1, 0b00100000, false));
    auto f2 = ChannelStatusUtils::read_flag(cs, sizeof(cs), 1, 0b00100000);
    ASSERT_TRUE(f2.has_value());
    EXPECT_FALSE(*f2);
}
