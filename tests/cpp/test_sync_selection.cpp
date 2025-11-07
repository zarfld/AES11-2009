#include <gtest/gtest.h>
#include "../../lib/Standards/AES/AES11/2009/sync/synchronization_manager.hpp"

using AES::AES11::_2009::sync::SynchronizationManager;
using AES::AES11::_2009::sync::SourceMetrics;

// Verifies: REQ-F-SYNC-001
// TEST-SYNC-SELECT-001: Selection with hysteresis retains current when improvement < margin.
TEST(SyncSelectionTests, SelectionAppliesHysteresis) {
    SynchronizationManager mgr(/*hysteresisMargin*/ 0.5);
    std::vector<SourceMetrics> sources = {
        {0.10, 5.0, false}, // stability, quality, degraded flag
        {0.20, 5.3, false},
        {0.05, 5.1, false}
    };
    // Initial selection: compute composite scores
    // score0 = 5.0 - 0.10 = 4.90
    // score1 = 5.3 - 0.20 = 5.10 (best)
    // score2 = 5.1 - 0.05 = 5.05
    size_t first = mgr.select(sources);
    EXPECT_EQ(first, 1u);

    // Slight improvement for source2 (< hysteresis margin difference vs current)
    sources[2].quality = 5.35; // score2 becomes 5.35 - 0.05 = 5.30; current score1 = 5.10
    // Difference = 0.20 < margin 0.5 → retain index 1
    size_t second = mgr.select(sources);
    EXPECT_EQ(second, 1u);

    // Larger improvement surpassing hysteresis
    sources[2].quality = 5.80; // new score2 = 5.80 - 0.05 = 5.75 (delta 0.65 > 0.5)
    size_t third = mgr.select(sources);
    EXPECT_EQ(third, 2u);
}

// Verifies: REQ-F-SYNC-001
// TEST-SYNC-SELECT-002: Degraded source penalization avoids selection despite nominal quality
TEST(SyncSelectionTests, DegradedSourcePenalized) {
    SynchronizationManager mgr(0.2);
    std::vector<SourceMetrics> sources = {
        {0.05, 6.0, true},  // degraded high quality
        {0.10, 5.5, false}, // non-degraded moderate quality
    };
    // scores: degraded -> 6.0 - 0.05 - 10.0 = -4.05, normal -> 5.5 - 0.10 = 5.40
    size_t sel = mgr.select(sources);
    EXPECT_EQ(sel, 1u);
}

// Verifies: REQ-F-SYNC-001
// TEST-SYNC-SELECT-003: Empty source list returns invalid index and preserves current
TEST(SyncSelectionTests, EmptySourceListReturnsInvalid) {
    SynchronizationManager mgr(0.2);
    std::vector<SourceMetrics> none;
    auto sel = mgr.select(none);
    EXPECT_EQ(sel, SynchronizationManager::invalidIndex());
    EXPECT_EQ(mgr.current(), SynchronizationManager::invalidIndex());
}

// Verifies: REQ-F-SYNC-001
// TEST-SYNC-SELECT-004: Tie scores retain current (hysteresis prevents switch)
TEST(SyncSelectionTests, TieScoresRetainCurrent) {
    SynchronizationManager mgr(0.1);
    std::vector<SourceMetrics> sources = {
        {0.10, 5.0, false}, // score 4.90
        {0.10, 5.2, false}  // score 5.10 (initial best)
    };
    size_t first = mgr.select(sources);
    EXPECT_EQ(first, 1u);
    // Adjust source0 to tie score of source1 but improvement below margin
    sources[0].quality = 5.2; // score now 5.10 equal
    size_t second = mgr.select(sources);
    EXPECT_EQ(second, 1u); // retain current
}

// Verifies: REQ-F-SYNC-001
// TEST-SYNC-SELECT-005: All sources degraded selects best adjusted score
TEST(SyncSelectionTests, AllSourcesDegradedSelectsHighestScore) {
    SynchronizationManager mgr(0.3);
    std::vector<SourceMetrics> sources = {
        {0.10, 6.0, true}, // score -4.10
        {0.05, 6.1, true}, // score -3.95 (best)
        {0.20, 6.3, true}  // score -3.90 (actually best -> 6.3-0.20-10 = -3.90)
    };
    // Correction: compute each precisely -> choose index 2
    size_t sel = mgr.select(sources);
    EXPECT_EQ(sel, 2u);
}

// Verifies: REQ-F-SYNC-001
// TEST-SYNC-SELECT-006: Improvement equal to margin retains current
TEST(SyncSelectionTests, ImprovementEqualsMarginRetainsCurrent) {
    SynchronizationManager mgr(0.20);
    std::vector<SourceMetrics> sources = {
        {0.10, 5.00, false}, // score 4.90 (index 0)
        {0.10, 5.05, false}, // score 4.95 (index 1 best initially)
    };
    size_t first = mgr.select(sources);
    ASSERT_EQ(first, 1u);
    // Improve source 0 so delta(best - current) == 0.20 (equal margin)
    // currentScore = 4.95 (idx1); make idx0 score = 5.15 to exceed? We want best improvement exactly 0.20
    // Set quality to 5.25: score0 = 5.25 - 0.10 = 5.15; delta = 5.15 - 4.95 = 0.20
    sources[0].quality = 5.25;
    size_t second = mgr.select(sources);
    EXPECT_EQ(second, 1u) << "Equal to margin should retain current";
}

// Verifies: REQ-F-SYNC-001
// TEST-SYNC-SELECT-007: Improvement greater than margin switches selection
TEST(SyncSelectionTests, ImprovementGreaterThanMarginSwitches) {
    SynchronizationManager mgr(0.20);
    std::vector<SourceMetrics> sources = {
        {0.10, 5.00, false}, // 4.90
        {0.10, 5.05, false}, // 4.95 (current)
    };
    size_t first = mgr.select(sources);
    ASSERT_EQ(first, 1u);
    // Make index 0 better by > margin
    sources[0].quality = 5.30; // score0 = 5.20; delta = 0.25 > 0.20
    size_t second = mgr.select(sources);
    EXPECT_EQ(second, 0u);
}


