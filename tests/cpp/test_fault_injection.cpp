// TEST-UNIT-FaultInjection
// Traceability:
//   Design: DES-C-004, DES-C-001
//   Requirements: PHASE05-RELIABILITY-HOOKS, REQ-F-CS-ANNEXA-DT
//   Notes: Validates AES11 fault injection compile flag and controlled failure paths.

#include <gtest/gtest.h>
#include <cstdint>
#include "Common/testing/fault_injection.hpp"
#include "Common/reliability/metrics.hpp"
// Align include path with other tests to resolve header correctly
#include "../../lib/Standards/AES/AES11/2009/core/channel_status_utils.hpp"

using AES::AES11::_2009::core::ChannelStatusUtils;
using AES::AES11::_2009::core::DateTimeFields;
using Common::testing::FaultInjection;
using Common::reliability::ReliabilityMetrics;

// Helper to determine if fault injection is compiled in.
static bool faultInjectionCompiled() {
#ifdef AES11_FAULT_INJECTION
    return true;
#else
    return false;
#endif
}

TEST(FaultInjectionTests, SetDateTimeInfoForcedFailure) {
    if (!faultInjectionCompiled()) {
        GTEST_SKIP() << "Fault injection not compiled (ENABLE_FAULT_INJECTION=OFF)";
    }
    ReliabilityMetrics::resetForTesting();
    uint8_t cs[24]{};
    DateTimeFields dt{25, 11, 5, 12, 30, 10, true, false};

    // First, ensure success when not enabled.
    FaultInjection::clear();
    ASSERT_TRUE(ChannelStatusUtils::set_datetime_info(cs, sizeof(cs), dt));
    auto snap1 = ReliabilityMetrics::snapshot();
    EXPECT_EQ(snap1.dateTimeFailures, 0u);

    // Enable fault point and assert failure.
    FaultInjection::enable("ChannelStatusUtils.set_datetime_info.fail");
    ASSERT_FALSE(ChannelStatusUtils::set_datetime_info(cs, sizeof(cs), dt));
    auto snap2 = ReliabilityMetrics::snapshot();
    EXPECT_GE(snap2.dateTimeFailures, 1u);
    FaultInjection::disable("ChannelStatusUtils.set_datetime_info.fail");
}

TEST(FaultInjectionTests, ExtractDateTimeInfoForcedFailure) {
    if (!faultInjectionCompiled()) {
        GTEST_SKIP() << "Fault injection not compiled (ENABLE_FAULT_INJECTION=OFF)";
    }
    ReliabilityMetrics::resetForTesting();
    uint8_t cs[24]{};
    DateTimeFields dt{25, 11, 5, 12, 30, 10, true, false};
    ASSERT_TRUE(ChannelStatusUtils::set_datetime_info(cs, sizeof(cs), dt));

    // Enable fault point and expect nullopt with failure increment.
    FaultInjection::enable("ChannelStatusUtils.extract_datetime_info.fail");
    auto extracted = ChannelStatusUtils::extract_datetime_info(cs, sizeof(cs));
    EXPECT_FALSE(extracted.has_value());
    auto snap = ReliabilityMetrics::snapshot();
    EXPECT_GE(snap.dateTimeFailures, 1u);
    FaultInjection::disable("ChannelStatusUtils.extract_datetime_info.fail");
}
