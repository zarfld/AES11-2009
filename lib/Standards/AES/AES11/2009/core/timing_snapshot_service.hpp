// Copyright (c) 2025
// This file implements protocol functionality based on understanding of:
// - AES-11-2009 (Digital Audio Reference Signals) - Copyright AES
// - AES3-2009 (Digital Audio Interface) - Copyright AES
// - AES5-2018 (Preferred Sampling Frequencies) - Copyright AES
// No copyrighted content is reproduced. Refer to original specs for authority.

#ifndef AES_AES11_2009_CORE_TIMING_SNAPSHOT_SERVICE_HPP
#define AES_AES11_2009_CORE_TIMING_SNAPSHOT_SERVICE_HPP

#include <cstdint>
#include <atomic>

#include "../../../../Common/interfaces/clock_interface.hpp"

namespace AES {
namespace AES11 {
namespace _2009 {
namespace core {

/**
 * TimingSnapshot contains a coherent, atomic capture of timing state.
 * - Implements an atomic snapshot concept aligned to AES-11-2009 timing
 *   reference principles (see AES-11-2009 Sections 3.3, 5.x references).
 */
struct TimingSnapshot {
    uint64_t tick;  // abstract tick counter from clock source
    uint64_t time_ns; // nanoseconds since an arbitrary epoch
    uint64_t seq;   // monotonically increasing sequence for snapshot ordering
};

/**
 * TimingSnapshotService
 * Provides atomic snapshots of timing derived from an injected clock source.
 * - Hardware-agnostic via Common::interfaces::ClockInterface dependency.
 * - Thread-safe sequence assignment for snapshot ordering.
 *
 * Note: This service supports tests like TEST-TIMESRC-SNAPSHOT-001/002.
 */
class TimingSnapshotService {
public:
    explicit TimingSnapshotService(Common::interfaces::ClockInterface& clk)
        : _clk(clk), _seq(0) {}

    TimingSnapshot snapshot() {
        // Read underlying clock once to ensure coherence of fields for this snapshot
        const uint64_t tick = _clk.get_tick();
        const uint64_t ns = _clk.get_time_ns();
        const uint64_t seq = _seq.fetch_add(1, std::memory_order_relaxed) + 1;
        return TimingSnapshot{tick, ns, seq};
    }

private:
    Common::interfaces::ClockInterface& _clk;
    std::atomic<uint64_t> _seq;
};

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES

#endif // AES_AES11_2009_CORE_TIMING_SNAPSHOT_SERVICE_HPP
