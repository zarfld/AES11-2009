// TEST-UNIT-ReliabilityMetrics
// Traceability:
//   Phase: 05-implementation
//   Design: DES-C-004 (Reliability Metrics Aggregation - placeholder)
//   Requirements: PHASE05-RELIABILITY-HOOKS (implicit), REQ-F-CS-ANNEXA-LEN, REQ-F-CS-ANNEXA-DT, REQ-F-CS-ANNEXA-TZ
//   Notes: Validates that failure paths increment reliability counters for buffer length and date/time validation failures.

#include <gtest/gtest.h>
#include <cstdint>
#include <cstring>

#include "../../lib/Standards/AES/AES11/2009/core/channel_status_utils.hpp"
#include "../../lib/Standards/Common/reliability/metrics.hpp"

using AES::AES11::_2009::core::ChannelStatusUtils;
using Common::reliability::ReliabilityMetrics;

TEST(ReliabilityMetricsTests, CountersStartAtZero) {
    ReliabilityMetrics::resetForTesting();
    auto snap = ReliabilityMetrics::snapshot();
    EXPECT_EQ(snap.utcFailures, 0u);
    EXPECT_EQ(snap.dateTimeFailures, 0u);
    EXPECT_EQ(snap.leapSecondFailures, 0u);
    EXPECT_EQ(snap.timezoneFailures, 0u);
}

TEST(ReliabilityMetricsTests, UtcFailureIncrementsCounter) {
    uint8_t small[10]{}; // insufficient length triggers failure
    AES::AES11::_2009::core::UTCInfo info{};
    EXPECT_FALSE(ChannelStatusUtils::set_utc_info(small, sizeof(small), info));
    auto snap = ReliabilityMetrics::snapshot();
    EXPECT_GE(snap.utcFailures, 1u);
}

TEST(ReliabilityMetricsTests, DateTimeFailureIncrementsCounter) {
    uint8_t buf[24]{}; // valid length but invalid date (month 13)
    AES::AES11::_2009::core::DateTimeFields dt{25, 13, 5, 12, 30, 10, true, false};
    EXPECT_FALSE(ChannelStatusUtils::set_datetime_info(buf, sizeof(buf), dt));
    auto snap = ReliabilityMetrics::snapshot();
    EXPECT_GE(snap.dateTimeFailures, 1u);
}

TEST(ReliabilityMetricsTests, LeapSecondRuleFailureIncrementsCounter) {
    ReliabilityMetrics::resetForTesting();
    uint8_t buf[24]{};
    // Invalid: leapSecond true but second != 59
    AES::AES11::_2009::core::DateTimeFields dt{25, 11, 5, 12, 30, 10, true, true};
    EXPECT_FALSE(ChannelStatusUtils::set_datetime_info(buf, sizeof(buf), dt));
    auto snap = ReliabilityMetrics::snapshot();
    EXPECT_GE(snap.leapSecondFailures, 1u);
    EXPECT_EQ(snap.dateTimeFailures, 1u); // still counts toward dateTimeFailures
}

TEST(ReliabilityMetricsTests, TimezoneOutOfRangeIncrementsCounter) {
    ReliabilityMetrics::resetForTesting();
    uint8_t buf[24]{};
    AES::AES11::_2009::core::UTCInfo info{};
    info.utcValid = true;
    info.leapSecondPending = false;
    info.timezoneOffsetMinutes = 2000; // out of range
    EXPECT_FALSE(ChannelStatusUtils::set_utc_info(buf, sizeof(buf), info));
    auto snap = ReliabilityMetrics::snapshot();
    EXPECT_GE(snap.timezoneFailures, 1u);
    EXPECT_GE(snap.utcFailures, 1u); // still part of utcFailures
}

TEST(ReliabilityMetricsTests, IndependentCountersAccumulate) {
    uint8_t small[10]{};
    AES::AES11::_2009::core::UTCInfo info{};
    ASSERT_FALSE(ChannelStatusUtils::set_utc_info(small, sizeof(small), info)); // fail 1
    ASSERT_FALSE(ChannelStatusUtils::set_utc_info(small, sizeof(small), info)); // fail 2

    uint8_t buf[24]{};
    AES::AES11::_2009::core::DateTimeFields dtInvalidMonth{25, 0, 5, 12, 30, 10, true, false}; // month 0 invalid
    ASSERT_FALSE(ChannelStatusUtils::set_datetime_info(buf, sizeof(buf), dtInvalidMonth)); // fail 1
    AES::AES11::_2009::core::DateTimeFields dtInvalidDay{25, 11, 0, 12, 30, 10, true, false}; // day 0 invalid
    ASSERT_FALSE(ChannelStatusUtils::set_datetime_info(buf, sizeof(buf), dtInvalidDay)); // fail 2

    auto snap = ReliabilityMetrics::snapshot();
    EXPECT_GE(snap.utcFailures, 2u);
    EXPECT_GE(snap.dateTimeFailures, 2u);
}
