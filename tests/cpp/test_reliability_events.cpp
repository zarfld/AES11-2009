// TEST-UNIT-ReliabilityEvents
// Traceability:
//   Phase: 05-implementation
//   Design: DES-C-004 (Reliability Metrics & Events)
//   Requirements: PHASE05-RELIABILITY-HOOKS
//   Notes: Validates that installing an event sink captures emitted reliability events.

#include <gtest/gtest.h>
#include <vector>
#include <string>

#include "../../lib/Standards/Common/reliability/events.hpp"
#include "../../lib/Standards/Common/reliability/metrics.hpp"
#include "../../lib/Standards/AES/AES11/2009/core/channel_status_utils.hpp"

using Common::reliability::ReliabilityEvent;
using Common::reliability::set_event_sink;
using Common::reliability::emit_event;
using Common::reliability::ReliabilityMetrics;

namespace {
    struct Captured { std::string name; uint64_t value; };
    static std::vector<Captured> g_events;

    void test_sink(const ReliabilityEvent& evt) {
        g_events.push_back({evt.name ? evt.name : std::string("null"), evt.value});
    }
}

TEST(ReliabilityEventsTests, SinkCapturesMetricEmissions) {
    g_events.clear();
    set_event_sink(&test_sink);
    ReliabilityMetrics::resetForTesting();

    // Trigger a UTC and timezone failure via invalid timezone
    uint8_t buf[24]{};
    AES::AES11::_2009::core::UTCInfo info{};
    info.utcValid = true;
    info.timezoneOffsetMinutes = 2000; // out of range → utc_failure + timezone_failure
    (void)AES::AES11::_2009::core::ChannelStatusUtils::set_utc_info(buf, sizeof(buf), info);

    // Trigger a leap-second rule failure via invalid second
    AES::AES11::_2009::core::DateTimeFields dt{25, 11, 5, 12, 30, 10, true, true};
    (void)AES::AES11::_2009::core::ChannelStatusUtils::set_datetime_info(buf, sizeof(buf), dt);

    // Basic containment checks (names may appear multiple times with >=1 occurrences)
    bool sawUtc = false, sawTz = false, sawLeap = false;
    for (const auto& e : g_events) {
        if (e.name == "utc_failure") sawUtc = true;
        if (e.name == "timezone_failure") sawTz = true;
        if (e.name == "leap_second_failure") sawLeap = true;
    }
    EXPECT_TRUE(sawUtc);
    EXPECT_TRUE(sawTz);
    EXPECT_TRUE(sawLeap);

    // Cleanup: remove sink
    set_event_sink(nullptr);
}
