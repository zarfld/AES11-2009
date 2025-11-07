#include <gtest/gtest.h>
#include "../../lib/Standards/AES/AES11/2009/core/timing_snapshot_service.hpp"
#include <chrono>

// Simple mock clock providing deterministic monotonic behavior
class MockClock : public Common::interfaces::ClockInterface {
public:
    uint64_t get_tick() override { return ++tick_; }
    uint64_t get_time_ns() override {
        // Simulate nanosecond progression tied to tick
        return tick_ * 1'000'000ULL; // 1 ms per tick for test simplicity
    }
private:
    uint64_t tick_ = 0;
};

// TEST-TIMESRC-SNAPSHOT-001: Monotonic sequence ordering
TEST(TimingSnapshotServiceTests, MonotonicSequenceIncrements) {
    MockClock clk;
    AES::AES11::_2009::core::TimingSnapshotService svc(clk);

    auto s1 = svc.snapshot();
    auto s2 = svc.snapshot();
    auto s3 = svc.snapshot();

    ASSERT_LT(s1.seq, s2.seq);
    ASSERT_LT(s2.seq, s3.seq);
    ASSERT_LT(s1.time_ns, s2.time_ns);
    ASSERT_LT(s2.time_ns, s3.time_ns);
}

// TEST-TIMESRC-SNAPSHOT-002: Coherence of fields within snapshot (tick/time consistent)
TEST(TimingSnapshotServiceTests, SnapshotFieldsCoherent) {
    MockClock clk;
    AES::AES11::_2009::core::TimingSnapshotService svc(clk);

    auto snap = svc.snapshot();
    // For this mock: time_ns should equal tick * 1ms
    ASSERT_EQ(snap.time_ns, snap.tick * 1'000'000ULL);
}
