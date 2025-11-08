// TEST-UNIT-ChannelStatusUtilsTests
// Traceability:
//   Requirements: REQ-F-CS-ANNEXA-LEN, REQ-F-CS-ANNEXA-DT, REQ-F-CS-ANNEXA-TZ, REQ-F-CS-ANNEXA-FLAGS
//   Design: DES-C-001
//   Notes: Validates grade roundtrip and compact DateTime encode/decode helpers.
#include <gtest/gtest.h>
#include "../../lib/Standards/AES/AES11/2009/core/channel_status_utils.hpp"

using AES::AES11::_2009::core::ChannelStatusUtils;
using AES::AES11::_2009::core::DARSGrade;
using AES::AES11::_2009::core::DateTimeFields;

TEST(ChannelStatusUtilsTests, GradeEncodeDecodeRoundTrip) {
    uint8_t cs[24]{}; // zero-init
    // Initially unknown
    EXPECT_EQ(ChannelStatusUtils::extract_grade(cs, sizeof(cs)), DARSGrade::Unknown);

    ASSERT_TRUE(ChannelStatusUtils::set_grade(cs, sizeof(cs), DARSGrade::Grade1));
    EXPECT_EQ(ChannelStatusUtils::extract_grade(cs, sizeof(cs)), DARSGrade::Grade1);

    ASSERT_TRUE(ChannelStatusUtils::set_grade(cs, sizeof(cs), DARSGrade::Grade2));
    EXPECT_EQ(ChannelStatusUtils::extract_grade(cs, sizeof(cs)), DARSGrade::Grade2);

    ASSERT_TRUE(ChannelStatusUtils::set_grade(cs, sizeof(cs), DARSGrade::Reserved));
    EXPECT_EQ(ChannelStatusUtils::extract_grade(cs, sizeof(cs)), DARSGrade::Reserved);

    ASSERT_TRUE(ChannelStatusUtils::set_grade(cs, sizeof(cs), DARSGrade::Unknown));
    EXPECT_EQ(ChannelStatusUtils::extract_grade(cs, sizeof(cs)), DARSGrade::Unknown);
}

// Explicitly cover reserved grade pattern handling and extraction mapping
TEST(ChannelStatusUtilsTests, ReservedGradePatternHandled) {
    uint8_t cs[24]{};
    // Directly set raw bits to 0b11 via API and verify extract returns Reserved
    ASSERT_TRUE(ChannelStatusUtils::set_grade(cs, sizeof(cs), DARSGrade::Reserved));
    EXPECT_EQ(ChannelStatusUtils::extract_grade(cs, sizeof(cs)), DARSGrade::Reserved);
}

TEST(ChannelStatusUtilsTests, DateTimeEncodeDecodeRoundTrip) {
    DateTimeFields dt{25, 11, 7, 14, 30, 45, true, false};
    uint8_t buf[6]{};
    ASSERT_TRUE(ChannelStatusUtils::encode_datetime(dt, buf));
    auto out = ChannelStatusUtils::decode_datetime(buf);
    ASSERT_TRUE(out.has_value());
    EXPECT_EQ(out->year, 25);
    EXPECT_EQ(out->month, 11);
    EXPECT_EQ(out->day, 7);
    EXPECT_EQ(out->hour, 14);
    EXPECT_EQ(out->minute, 30);
    EXPECT_EQ(out->second, 45);
}

TEST(ChannelStatusUtilsTests, DateTimeValidationRejectsOutOfRange) {
    DateTimeFields bad{100, 13, 32, 24, 60, 60, true, false};
    uint8_t buf[6];
    EXPECT_FALSE(ChannelStatusUtils::encode_datetime(bad, buf));
}
