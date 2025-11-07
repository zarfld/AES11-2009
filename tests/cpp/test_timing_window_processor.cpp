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
