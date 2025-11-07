#include <gtest/gtest.h>
#include "../../lib/Standards/AES/AES11/2009/core/timing_snapshot_service.hpp"
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>
#include <algorithm>

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

// Verifies: REQ-NF-REL-004
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

// Verifies: REQ-NF-REL-004
// TEST-TIMESRC-SNAPSHOT-002: Coherence of fields within snapshot (tick/time consistent)
TEST(TimingSnapshotServiceTests, SnapshotFieldsCoherent) {
    MockClock clk;
    AES::AES11::_2009::core::TimingSnapshotService svc(clk);

    auto snap = svc.snapshot();
    // For this mock: time_ns should equal tick * 1ms
    ASSERT_EQ(snap.time_ns, snap.tick * 1'000'000ULL);
}

// Verifies: REQ-NF-REL-004
// TEST-TIMESRC-SNAPSHOT-003: Sequence growth no wrap in practical range
TEST(TimingSnapshotServiceTests, SequenceGrowthNoWrapInRange) {
    MockClock clk;
    AES::AES11::_2009::core::TimingSnapshotService svc(clk);
    const int iterations = 1000; // practical bound
    uint64_t lastSeq = 0;
    for (int i = 0; i < iterations; ++i) {
        auto s = svc.snapshot();
        ASSERT_GT(s.seq, lastSeq);
        lastSeq = s.seq;
    }
}

// Verifies: REQ-NF-REL-004
// TEST-TIMESRC-SNAPSHOT-004: Concurrent snapshots yield unique sequences
TEST(TimingSnapshotServiceTests, ConcurrentSnapshotsYieldUniqueSequences) {
    class ThreadClock : public Common::interfaces::ClockInterface {
    public:
        uint64_t get_tick() override { return ++tick_; }
        uint64_t get_time_ns() override { return ++time_ns_; }
    private:
        std::atomic<uint64_t> tick_{0};
        std::atomic<uint64_t> time_ns_{0};
    };

    ThreadClock clk;
    AES::AES11::_2009::core::TimingSnapshotService svc(clk);

    constexpr int threads = 4;
    constexpr int perThread = 100;
    std::vector<uint64_t> seqs;
    seqs.reserve(threads * perThread);
    std::mutex m;

    auto worker = [&]() {
        for (int i = 0; i < perThread; ++i) {
            auto s = svc.snapshot();
            std::lock_guard<std::mutex> lg(m);
            seqs.push_back(s.seq);
        }
    };

    std::vector<std::thread> ts;
    for (int i = 0; i < threads; ++i) ts.emplace_back(worker);
    for (auto& t : ts) t.join();

    std::sort(seqs.begin(), seqs.end());
    seqs.erase(std::unique(seqs.begin(), seqs.end()), seqs.end());
    EXPECT_EQ(seqs.size(), static_cast<size_t>(threads * perThread));
}
