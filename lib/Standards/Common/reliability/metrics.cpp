/*
Module: lib/Standards/Common/reliability/metrics.cpp
Phase: 05-implementation
Traceability:
    Design: DES-C-004 (Reliability Metrics Aggregation)
    Requirements: PHASE05-RELIABILITY-HOOKS, REQ-F-CS-ANNEXA-LEN, REQ-F-CS-ANNEXA-DT, REQ-F-CS-ANNEXA-TZ
    Tests: TEST-UNIT-ReliabilityMetrics
Notes: Implements simple atomic counters for reliability evidence; hardware/OS agnostic.
*/
#include "metrics.hpp"
#include "events.hpp"
#include <atomic>

namespace Common {
namespace reliability {

static std::atomic<uint64_t> g_utcFailures{0};
static std::atomic<uint64_t> g_dateTimeFailures{0};
static std::atomic<uint64_t> g_leapSecondFailures{0};
static std::atomic<uint64_t> g_timezoneFailures{0};

void ReliabilityMetrics::incrementUtcFailure() {
    g_utcFailures.fetch_add(1, std::memory_order_relaxed);
    emit_event({"utc_failure", 1, nullptr});
}

void ReliabilityMetrics::incrementDateTimeFailure() {
    g_dateTimeFailures.fetch_add(1, std::memory_order_relaxed);
    emit_event({"datetime_failure", 1, nullptr});
}

void ReliabilityMetrics::incrementLeapSecondFailure() {
    g_leapSecondFailures.fetch_add(1, std::memory_order_relaxed);
    emit_event({"leap_second_failure", 1, nullptr});
}

void ReliabilityMetrics::incrementTimezoneFailure() {
    g_timezoneFailures.fetch_add(1, std::memory_order_relaxed);
    emit_event({"timezone_failure", 1, nullptr});
}

MetricsSnapshot ReliabilityMetrics::snapshot() {
    MetricsSnapshot s{};
    s.utcFailures = g_utcFailures.load(std::memory_order_relaxed);
    s.dateTimeFailures = g_dateTimeFailures.load(std::memory_order_relaxed);
    s.leapSecondFailures = g_leapSecondFailures.load(std::memory_order_relaxed);
    s.timezoneFailures = g_timezoneFailures.load(std::memory_order_relaxed);
    return s;
}

void ReliabilityMetrics::resetForTesting() {
    g_utcFailures.store(0, std::memory_order_relaxed);
    g_dateTimeFailures.store(0, std::memory_order_relaxed);
    g_leapSecondFailures.store(0, std::memory_order_relaxed);
    g_timezoneFailures.store(0, std::memory_order_relaxed);
}

} // namespace reliability
} // namespace Common
