#include <gtest/gtest.h>
#include "../../lib/Standards/AES/AES11/2009/core/timing_window_processor.hpp"
#include <cmath>

using AES::AES11::_2009::core::TimingWindowProcessor;

// TEST-DM-TIMINGWIN-001: Correct mean and variance updates (including sliding window)
TEST(TimingWindowProcessorTests, MeanAndVarianceUpdates) {
    TimingWindowProcessor proc(4, /*varianceThreshold*/ 0.5);

    proc.addSample(1.0);
    proc.addSample(2.0);
    proc.addSample(3.0);
    proc.addSample(4.0);
    auto m = proc.metrics();
    EXPECT_EQ(m.count, 4u);
    EXPECT_NEAR(m.mean, 2.5, 1e-9);
    // For [1,2,3,4], population variance = M2/n = 5/4 = 1.25
    EXPECT_NEAR(m.variance, 1.25, 1e-9);

    // Sliding: capacity 4, add another -> window becomes [2,3,4,5]
    proc.addSample(5.0);
    m = proc.metrics();
    EXPECT_EQ(m.count, 4u);
    EXPECT_NEAR(m.mean, 3.5, 1e-9);
    // [2,3,4,5] population variance also 1.25
    EXPECT_NEAR(m.variance, 1.25, 1e-9);
}

// TEST-DM-TIMINGWIN-002: Stability flag under threshold and reaction to disturbance
TEST(TimingWindowProcessorTests, StabilityFlagBehavior) {
    // Small variance threshold to detect instability
    TimingWindowProcessor proc(5, /*varianceThreshold*/ 0.01);

    proc.addSample(1.00);
    proc.addSample(1.05);
    proc.addSample(0.95);
    proc.addSample(1.02);
    proc.addSample(0.98);
    auto m = proc.metrics();
    EXPECT_EQ(m.count, 5u);
    EXPECT_TRUE(m.stable) << "Variance should be below threshold for tight cluster";

    // Introduce disturbance; window slides, expect instability
    proc.addSample(1.50);
    m = proc.metrics();
    EXPECT_EQ(m.count, 5u);
    EXPECT_FALSE(m.stable) << "Variance should exceed threshold after disturbance";
}

// Edge Case: Capacity 1 window should always yield variance 0 and count 1.
TEST(TimingWindowProcessorTests, CapacityOneWindow) {
    TimingWindowProcessor proc(1, 0.01);
    proc.addSample(10.0);
    auto m = proc.metrics();
    EXPECT_EQ(m.count, 1u);
    EXPECT_NEAR(m.mean, 10.0, 1e-12);
    EXPECT_NEAR(m.variance, 0.0, 1e-12);
    EXPECT_TRUE(m.stable);
    // Add another sample; window slides, still variance zero.
    proc.addSample(20.0);
    m = proc.metrics();
    EXPECT_EQ(m.count, 1u);
    EXPECT_NEAR(m.mean, 20.0, 1e-12);
    EXPECT_NEAR(m.variance, 0.0, 1e-12);
}

// Edge Case: Clear resets aggregates.
TEST(TimingWindowProcessorTests, ClearResetsAggregates) {
    TimingWindowProcessor proc(4, 1.0);
    proc.addSample(1.0);
    proc.addSample(2.0);
    proc.clear();
    auto m = proc.metrics();
    EXPECT_EQ(m.count, 0u);
    EXPECT_NEAR(m.mean, 0.0, 1e-12);
    EXPECT_NEAR(m.variance, 0.0, 1e-12);
}

// Boundary: variance equal to threshold should be considered NOT stable (strict < used)
TEST(TimingWindowProcessorTests, VarianceEqualThresholdNotStable) {
    TimingWindowProcessor proc(4, /*varianceThreshold*/ 1.25);
    // Populate with [1,2,3,4] -> variance = 1.25 exactly
    proc.addSample(1.0);
    proc.addSample(2.0);
    proc.addSample(3.0);
    proc.addSample(4.0);
    auto m = proc.metrics();
    EXPECT_NEAR(m.variance, 1.25, 1e-9);
    EXPECT_FALSE(m.stable) << "Equality should not be marked stable (uses < threshold)";
}

// Robustness: negative and large values do not break mean/variance computation
TEST(TimingWindowProcessorTests, HandlesNegativeAndLargeValues) {
    TimingWindowProcessor proc(5, /*varianceThreshold*/ 1e12);
    proc.addSample(-1e6);
    proc.addSample(0.0);
    proc.addSample(1e6);
    proc.addSample(5.0);
    proc.addSample(-5.0);
    auto m = proc.metrics();
    EXPECT_EQ(m.count, 5u);
    // Basic sanity checks
    EXPECT_TRUE(std::isfinite(m.mean));
    EXPECT_TRUE(std::isfinite(m.variance));
}

