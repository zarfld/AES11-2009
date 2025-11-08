// Tests for REQ-F-DARS-007: Date and Time Distribution via Channel Status (Annex A)
// RED phase of TDD: These tests will initially fail until implementation added.
// No copyrighted AES-11 content reproduced; logic based on original understanding.
// Traceability:
//   Requirements: REQ-F-CS-ANNEXA-DT, REQ-F-CS-ANNEXA-LEN, REQ-F-CS-ANNEXA-LEAP
//   Design: DES-C-001
//   Tests: TEST-UNIT-DateTimeChannelStatus

#include <gtest/gtest.h>
#include "../../lib/Standards/AES/AES11/2009/core/channel_status_utils.hpp"

using AES::AES11::_2009::core::ChannelStatusUtils;
using AES::AES11::_2009::core::DateTimeFields;

namespace {
    // Helper to allocate a standard channel status buffer (24 bytes typical)
    std::array<uint8_t,24> makeBuffer() { std::array<uint8_t,24> b{}; return b; }
}

TEST(ChannelStatusDateTimeTests, EncodeDecodeUtcHappyPath) {
    auto buf = makeBuffer();
    DateTimeFields dt{25, 11, 7, 14, 30, 45, true, false}; // 2025-11-07 14:30:45 UTC
    ASSERT_TRUE(ChannelStatusUtils::set_datetime_info(buf.data(), buf.size(), dt));
    auto outOpt = ChannelStatusUtils::extract_datetime_info(buf.data(), buf.size());
    ASSERT_TRUE(outOpt.has_value());
    auto out = *outOpt;
    EXPECT_EQ(out.year, dt.year);
    EXPECT_EQ(out.month, dt.month);
    EXPECT_EQ(out.day, dt.day);
    EXPECT_EQ(out.hour, dt.hour);
    EXPECT_EQ(out.minute, dt.minute);
    EXPECT_EQ(out.second, dt.second);
    EXPECT_TRUE(out.utc);
    EXPECT_FALSE(out.leapSecond);
}

TEST(ChannelStatusDateTimeTests, EncodeDecodeLeapSecondBoundary) {
    auto buf = makeBuffer();
    // Leap second uses second=59 with leapSecond flag true (avoid storing 60)
    DateTimeFields dt{25, 12, 31, 23, 59, 59, true, true};
    ASSERT_TRUE(ChannelStatusUtils::set_datetime_info(buf.data(), buf.size(), dt));
    auto outOpt = ChannelStatusUtils::extract_datetime_info(buf.data(), buf.size());
    ASSERT_TRUE(outOpt.has_value());
    auto out = *outOpt;
    EXPECT_TRUE(out.leapSecond);
    EXPECT_EQ(out.second, 59); // Represent leap second with flag
}

// Leap second flag must only be true when second == 59 (representation of leap boundary)
TEST(ChannelStatusDateTimeTests, RejectLeapSecondFlagWhenSecondNot59) {
    auto buf = makeBuffer();
    // Invalid: leapSecond true but second=58
    DateTimeFields dt{25, 11, 7, 14, 30, 58, true, true};
    EXPECT_FALSE(ChannelStatusUtils::set_datetime_info(buf.data(), buf.size(), dt));
    // Should not have modified buffer to contain leapSecond flag
    auto outOpt = ChannelStatusUtils::extract_datetime_info(buf.data(), buf.size());
    // Either no value (invalid extraction) or leapSecond false; we assert invalid set prevented flag
    if (outOpt.has_value()) {
        EXPECT_FALSE(outOpt->leapSecond);
    }
}

TEST(ChannelStatusDateTimeTests, RejectInvalidMonth) {
    auto buf = makeBuffer();
    DateTimeFields dt{25, 13, 7, 14, 30, 45, true, false};
    EXPECT_FALSE(ChannelStatusUtils::set_datetime_info(buf.data(), buf.size(), dt));
}

TEST(ChannelStatusDateTimeTests, RejectSecond60WithoutLeapFlag) {
    auto buf = makeBuffer();
    DateTimeFields dt{25, 11, 7, 14, 30, 60, true, false}; // invalid second
    EXPECT_FALSE(ChannelStatusUtils::set_datetime_info(buf.data(), buf.size(), dt));
}

TEST(ChannelStatusDateTimeTests, MinimumBufferLengthCheck) {
    uint8_t small[10]{}; // too small for assumed mapping
    DateTimeFields dt{25, 11, 7, 14, 30, 45, true, false};
    EXPECT_FALSE(ChannelStatusUtils::set_datetime_info(small, sizeof(small), dt));
    EXPECT_FALSE(ChannelStatusUtils::extract_datetime_info(small, sizeof(small)).has_value());
}
