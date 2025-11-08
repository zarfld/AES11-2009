// TEST-UNIT-ChannelStatusAnnexATests
// Traceability:
//   Requirements: REQ-F-CS-ANNEXA-LEN, REQ-F-CS-ANNEXA-TZ, REQ-F-CS-ANNEXA-FLAGS, REQ-F-CS-ANNEXA-DT
//   Design: DES-C-001
//   Notes: Validates UTC/TZ roundtrips, flag read/write behavior, preservation of unmapped bytes, and length enforcement.
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

// New RED tests: enforce minimum channel status length (kChannelStatusMinBytes)
// Currently implementation only checks highest accessed index; these tests codify
// REQ-F-CS-ANNEXA-LEN that any Annex A mapping operation requires full 24-byte block.
// Expect failure for sub-minimum buffers (e.g., length 20) even though indices are in-range.
TEST(ChannelStatusAnnexATests, RejectsSubMinimumLengthBufferUtc) {
    uint8_t partial[20]{}; // indices up to 19 exist but < required 24 bytes
    UTCInfo in{};
    in.utcValid = true;
    in.leapSecondPending = false;
    in.timezoneOffsetMinutes = 0;
    EXPECT_FALSE(ChannelStatusUtils::set_utc_info(partial, sizeof(partial), in));
    EXPECT_FALSE(ChannelStatusUtils::extract_utc_info(partial, sizeof(partial)).has_value());
}

TEST(ChannelStatusAnnexATests, RejectsSubMinimumLengthBufferDateTime) {
    uint8_t partial[20]{}; // length below kChannelStatusMinBytes
    AES::AES11::_2009::core::DateTimeFields dt{25, 11, 7, 14, 30, 45, true, false};
    EXPECT_FALSE(ChannelStatusUtils::set_datetime_info(partial, sizeof(partial), dt));
    EXPECT_FALSE(ChannelStatusUtils::extract_datetime_info(partial, sizeof(partial)).has_value());
}

TEST(ChannelStatusAnnexATests, RejectsZeroLengthBufferUtc) {
    uint8_t* none = nullptr; // simulate null pointer usage separately
    UTCInfo in{};
    EXPECT_FALSE(ChannelStatusUtils::set_utc_info(nullptr, 0, in));
    EXPECT_FALSE(ChannelStatusUtils::extract_utc_info(nullptr, 0).has_value());
    uint8_t zeroLen[1]{}; // length 1 still below minimum
    EXPECT_FALSE(ChannelStatusUtils::set_utc_info(zeroLen, 0, in));
    EXPECT_FALSE(ChannelStatusUtils::extract_utc_info(zeroLen, 0).has_value());
}

TEST(ChannelStatusAnnexATests, RejectsZeroLengthBufferDateTime) {
    AES::AES11::_2009::core::DateTimeFields dt{25, 11, 7, 14, 30, 45, true, false};
    EXPECT_FALSE(ChannelStatusUtils::set_datetime_info(nullptr, 0, dt));
    EXPECT_FALSE(ChannelStatusUtils::extract_datetime_info(nullptr, 0).has_value());
    uint8_t buf[1]{};
    EXPECT_FALSE(ChannelStatusUtils::set_datetime_info(buf, 0, dt));
    EXPECT_FALSE(ChannelStatusUtils::extract_datetime_info(buf, 0).has_value());
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

// New: Non-audio/content-type flag roundtrip, implemented as an internal mapping
TEST(ChannelStatusAnnexATests, NonAudioFlagRoundTrip) {
    uint8_t cs[24]{};
    // Default should be false
    auto n0 = ChannelStatusUtils::read_non_audio(cs, sizeof(cs));
    ASSERT_TRUE(n0.has_value());
    EXPECT_FALSE(*n0);

    // Set to true and read back
    ASSERT_TRUE(ChannelStatusUtils::set_non_audio(cs, sizeof(cs), true));
    auto n1 = ChannelStatusUtils::read_non_audio(cs, sizeof(cs));
    ASSERT_TRUE(n1.has_value());
    EXPECT_TRUE(*n1);

    // Clear to false again
    ASSERT_TRUE(ChannelStatusUtils::set_non_audio(cs, sizeof(cs), false));
    auto n2 = ChannelStatusUtils::read_non_audio(cs, sizeof(cs));
    ASSERT_TRUE(n2.has_value());
    EXPECT_FALSE(*n2);
}

// New: Alignment marker flag roundtrip (implementation-defined bit mapping)
TEST(ChannelStatusAnnexATests, AlignmentMarkerFlagRoundTrip) {
    uint8_t cs[24]{};
    // Expect default false
    auto a0 = ChannelStatusUtils::read_alignment_marker(cs, sizeof(cs));
    ASSERT_TRUE(a0.has_value());
    EXPECT_FALSE(*a0);

    // Set true
    ASSERT_TRUE(ChannelStatusUtils::set_alignment_marker(cs, sizeof(cs), true));
    auto a1 = ChannelStatusUtils::read_alignment_marker(cs, sizeof(cs));
    ASSERT_TRUE(a1.has_value());
    EXPECT_TRUE(*a1);

    // Clear
    ASSERT_TRUE(ChannelStatusUtils::set_alignment_marker(cs, sizeof(cs), false));
    auto a2 = ChannelStatusUtils::read_alignment_marker(cs, sizeof(cs));
    ASSERT_TRUE(a2.has_value());
    EXPECT_FALSE(*a2);
}
