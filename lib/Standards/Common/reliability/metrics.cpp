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
#include <atomic>

namespace Common {
namespace reliability {

static std::atomic<uint64_t> g_utcFailures{0};
static std::atomic<uint64_t> g_dateTimeFailures{0};

void ReliabilityMetrics::incrementUtcFailure() {
    g_utcFailures.fetch_add(1, std::memory_order_relaxed);
}

void ReliabilityMetrics::incrementDateTimeFailure() {
    g_dateTimeFailures.fetch_add(1, std::memory_order_relaxed);
}

MetricsSnapshot ReliabilityMetrics::snapshot() {
    MetricsSnapshot s{};
    s.utcFailures = g_utcFailures.load(std::memory_order_relaxed);
    s.dateTimeFailures = g_dateTimeFailures.load(std::memory_order_relaxed);
    return s;
}

} // namespace reliability
} // namespace Common
