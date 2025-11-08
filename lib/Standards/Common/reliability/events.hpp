/*
Module: lib/Standards/Common/reliability/events.hpp
Phase: 05-implementation
Traceability:
    Design: DES-C-004 (Reliability Metrics & Events)
    Requirements: PHASE05-RELIABILITY-HOOKS
    Tests: TEST-UNIT-ReliabilityEvents
Notes: Minimal structured event sink for reliability evidence. No-op by default; hardware/OS agnostic.
*/
#ifndef STANDARDS_COMMON_RELIABILITY_EVENTS_HPP
#define STANDARDS_COMMON_RELIABILITY_EVENTS_HPP

#include <cstdint>

namespace Common {
namespace reliability {

struct ReliabilityEvent {
    const char* name;      // e.g., "utc_failure"
    uint64_t value;        // typically 1 per occurrence
    const char* context;   // optional textual context (may be nullptr)
};

using ReliabilityEventSink = void(*)(const ReliabilityEvent&);

// Register a global event sink (thread-safe). Passing nullptr disables events.
void set_event_sink(ReliabilityEventSink sink);

// Emit an event to the current sink, if any.
void emit_event(const ReliabilityEvent& evt);

} // namespace reliability
} // namespace Common

#endif // STANDARDS_COMMON_RELIABILITY_EVENTS_HPP
