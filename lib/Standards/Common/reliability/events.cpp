/*
Module: lib/Standards/Common/reliability/events.cpp
Phase: 05-implementation
Traceability:
    Design: DES-C-004 (Reliability Metrics & Events)
    Requirements: PHASE05-RELIABILITY-HOOKS
    Tests: TEST-UNIT-ReliabilityEvents
Notes: Implements global event sink as a function pointer. If unset, emit_event is a no-op.
*/
#include "events.hpp"
#include <atomic>

namespace Common {
namespace reliability {

static std::atomic<ReliabilityEventSink> g_sink{nullptr};

void set_event_sink(ReliabilityEventSink sink) {
    g_sink.store(sink, std::memory_order_relaxed);
}

void emit_event(const ReliabilityEvent& evt) {
    auto sink = g_sink.load(std::memory_order_relaxed);
    if (sink) {
        sink(evt);
    }
}

} // namespace reliability
} // namespace Common
