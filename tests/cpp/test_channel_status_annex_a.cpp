#include <gtest/gtest.h>
#include <cstring>
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

// New: DST flag roundtrip using implementation-defined mapping
TEST(ChannelStatusAnnexATests, DstFlagRoundTrip) {
    uint8_t cs[24]{};
    // Initially false
    auto f0 = ChannelStatusUtils::read_dst_flag(cs, sizeof(cs));
    ASSERT_TRUE(f0.has_value());
    EXPECT_FALSE(*f0);
    // Set and read back
    ASSERT_TRUE(ChannelStatusUtils::set_dst_flag(cs, sizeof(cs), true));
    auto f1 = ChannelStatusUtils::read_dst_flag(cs, sizeof(cs));
    ASSERT_TRUE(f1.has_value());
    EXPECT_TRUE(*f1);
    // Clear again
    ASSERT_TRUE(ChannelStatusUtils::set_dst_flag(cs, sizeof(cs), false));
    auto f2 = ChannelStatusUtils::read_dst_flag(cs, sizeof(cs));
    ASSERT_TRUE(f2.has_value());
    EXPECT_FALSE(*f2);
}

// Reserved bits preservation: writing UTC/DST/DateTime should not alter untouched bytes
TEST(ChannelStatusAnnexATests, PreservesReservedBytesOutsideMappedFields) {
    uint8_t cs[24];
    // Pre-fill with a pattern
    for (int i = 0; i < 24; ++i) cs[i] = static_cast<uint8_t>(0xA5 ^ i);

    // Make a copy for later comparison
    uint8_t before[24];
    std::memcpy(before, cs, sizeof(cs));

    // Apply UTC info, DST, and DateTime
    UTCInfo in{};
    in.utcValid = true;
    in.leapSecondPending = true;
    in.timezoneOffsetMinutes = 60;
    ASSERT_TRUE(ChannelStatusUtils::set_utc_info(cs, sizeof(cs), in));

    ASSERT_TRUE(ChannelStatusUtils::set_dst_flag(cs, sizeof(cs), true));

    AES::AES11::_2009::core::DateTimeFields dt{25, 11, 7, 14, 30, 59, true, true};
    ASSERT_TRUE(ChannelStatusUtils::set_datetime_info(cs, sizeof(cs), dt));

    // Verify that bytes unrelated to our mapped fields remain unchanged
    // Mapped bytes: 11..16 (date/time), 17 (flags), 18..19 (tz)
    for (int i = 0; i < 24; ++i) {
        bool is_mapped = (i >= 11 && i <= 16) || (i == 17) || (i == 18) || (i == 19);
        if (!is_mapped) {
            EXPECT_EQ(cs[i], before[i]) << "Byte index " << i << " should be preserved";
        }
    }
}
