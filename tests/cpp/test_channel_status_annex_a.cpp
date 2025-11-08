#include <gtest/gtest.h>
#include "../../lib/Standards/AES/AES11/2009/core/channel_status_utils.hpp"

using AES::AES11::_2009::core::ChannelStatusUtils;
using AES::AES11::_2009::core::UTCInfo;

// These tests validate our implementation-level Annex A mapping behavior.
// They DO NOT reproduce any AES-11 copyrighted content and rely on original
// logic with safe field assumptions documented in the header.

TEST(ChannelStatusAnnexATests, UtcInfoRoundTripWithinBounds) {
    // Minimal channel status buffer accommodating assumed indices up to 20
    uint8_t cs[24]{};

    UTCInfo in{};
    in.utcValid = true;
    in.leapSecondPending = false;
    in.timezoneOffsetMinutes = 120; // UTC+02:00

    ASSERT_TRUE(ChannelStatusUtils::set_utc_info(cs, sizeof(cs), in));
    auto outOpt = ChannelStatusUtils::extract_utc_info(cs, sizeof(cs));
    ASSERT_TRUE(outOpt.has_value());

    auto out = *outOpt;
    EXPECT_TRUE(out.utcValid);
    EXPECT_FALSE(out.leapSecondPending);
    EXPECT_EQ(out.timezoneOffsetMinutes, 120);
}

TEST(ChannelStatusAnnexATests, UtcInfoNegativeOffsetRoundTrip) {
    uint8_t cs[24]{};

    UTCInfo in{};
    in.utcValid = true;
    in.leapSecondPending = true;
    in.timezoneOffsetMinutes = -300; // UTC-05:00

    ASSERT_TRUE(ChannelStatusUtils::set_utc_info(cs, sizeof(cs), in));
    auto outOpt = ChannelStatusUtils::extract_utc_info(cs, sizeof(cs));
    ASSERT_TRUE(outOpt.has_value());

    auto out = *outOpt;
    EXPECT_TRUE(out.utcValid);
    EXPECT_TRUE(out.leapSecondPending);
    EXPECT_EQ(out.timezoneOffsetMinutes, -300);
}

TEST(ChannelStatusAnnexATests, RejectsOutOfRangeTimezone) {
    uint8_t cs[24]{};

    UTCInfo in{};
    in.utcValid = false;
    in.leapSecondPending = false;
    in.timezoneOffsetMinutes = 900; // beyond +14:00

    EXPECT_FALSE(ChannelStatusUtils::set_utc_info(cs, sizeof(cs), in));
}

TEST(ChannelStatusAnnexATests, RejectsInsufficientBuffer) {
    uint8_t small[10]{};
    UTCInfo in{};
    EXPECT_FALSE(ChannelStatusUtils::set_utc_info(small, sizeof(small), in));
    EXPECT_FALSE(ChannelStatusUtils::extract_utc_info(small, sizeof(small)).has_value());
}
