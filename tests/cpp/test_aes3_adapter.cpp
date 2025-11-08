#include <gtest/gtest.h>
#include "../../lib/Standards/AES/AES11/2009/core/aes3_adapter.hpp"

using AES::AES11::_2009::core::AES3Adapter;
using AES::AES11::_2009::core::TRPInfo;

TEST(AES3AdapterTests, DetectPreambleHeuristicBehaves) {
    uint8_t frame[4] = {0xFF, 0x00, 0xAA, 0x55};
    EXPECT_TRUE(AES3Adapter::detect_preamble(frame, 32));

    uint8_t zeros[4] = {0,0,0,0};
    EXPECT_FALSE(AES3Adapter::detect_preamble(zeros, 32));
}

TEST(AES3AdapterTests, ExtractTrpScaffolding) {
    uint8_t frame[4] = {0x12, 0x34, 0x56, 0x78};
    TRPInfo info = AES3Adapter::extract_trp(frame, 32);
    EXPECT_TRUE(info.valid);
}
