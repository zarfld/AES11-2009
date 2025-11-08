#include <gtest/gtest.h>

#include "AES/AES11/2009/core/aes5_adapter.hpp"

using AES::AES11::_2009::core::AES5Adapter;

// These tests only apply when AES5 integration is enabled via submodule and CMake define.
#ifdef AES5_INTEGRATION

TEST(AES5AdapterTests, PrimaryFrequencyRecognition) {
    EXPECT_TRUE(AES5Adapter::is_primary(48000));
    EXPECT_FALSE(AES5Adapter::is_primary(44100));
}

TEST(AES5AdapterTests, StandardRatesRecognition) {
    EXPECT_TRUE(AES5Adapter::is_standard_rate(32000));
    EXPECT_TRUE(AES5Adapter::is_standard_rate(44100));
    EXPECT_TRUE(AES5Adapter::is_standard_rate(48000));
    EXPECT_TRUE(AES5Adapter::is_standard_rate(96000));
    EXPECT_TRUE(AES5Adapter::is_standard_rate(192000));
    EXPECT_FALSE(AES5Adapter::is_standard_rate(12345));
}

#endif // AES5_INTEGRATION
