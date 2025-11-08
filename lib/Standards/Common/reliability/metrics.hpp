/*
Module: lib/Standards/Common/reliability/metrics.hpp
Phase: 05-implementation
Traceability:
    Design: DES-C-004 (Reliability Metrics Aggregation)
    Requirements: PHASE05-RELIABILITY-HOOKS, REQ-F-CS-ANNEXA-LEN, REQ-F-CS-ANNEXA-DT, REQ-F-CS-ANNEXA-TZ
    Tests: TEST-UNIT-ReliabilityMetrics
Notes: Provides simple thread-safe counters for failure paths to support IEEE 1633 reliability evidence.
*/
#ifndef STANDARDS_COMMON_RELIABILITY_METRICS_HPP
#define STANDARDS_COMMON_RELIABILITY_METRICS_HPP

#include <cstdint>

namespace Common {
namespace reliability {

struct MetricsSnapshot {
    uint64_t utcFailures{0};
    uint64_t dateTimeFailures{0};
    uint64_t leapSecondFailures{0};
    uint64_t timezoneFailures{0};
};

class ReliabilityMetrics {
public:
    // Increment counters
    static void incrementUtcFailure();
    static void incrementDateTimeFailure();
    static void incrementLeapSecondFailure();
    static void incrementTimezoneFailure();

    // Return current values (non-resetting)
    static MetricsSnapshot snapshot();

    // Test support: reset all counters to zero for deterministic assertions
    static void resetForTesting();
};

} // namespace reliability
} // namespace Common

#endif // STANDARDS_COMMON_RELIABILITY_METRICS_HPP
