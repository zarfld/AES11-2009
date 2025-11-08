#include <gtest/gtest.h>
#include "../../lib/Standards/AES/AES11/2009/core/phase_tolerance.hpp"

using AES::AES11::_2009::core::PhaseTolerance;

// Verifies: REQ-F-DARS-004
// TEST-DARS-PHASE-001: Output tolerance boundary at 48 kHz
TEST(PhaseToleranceTests, OutputToleranceBoundary48k) {
    double sr = 48000.0;
    double framePeriodUs = PhaseTolerance::frame_period_us(sr);
    double tolUs = PhaseTolerance::output_tolerance_us(sr);
    // Exactly at tolerance should pass
    EXPECT_TRUE(PhaseTolerance::within_output(sr, tolUs));
    // Slightly above tolerance should fail
    EXPECT_FALSE(PhaseTolerance::within_output(sr, tolUs * 1.01));
}

// Verifies: REQ-F-DARS-004
// TEST-DARS-PHASE-002: Input tolerance boundary at 48 kHz
TEST(PhaseToleranceTests, InputToleranceBoundary48k) {
    double sr = 48000.0;
    double tolUs = PhaseTolerance::input_tolerance_us(sr);
    EXPECT_TRUE(PhaseTolerance::within_input(sr, tolUs));
    EXPECT_FALSE(PhaseTolerance::within_input(sr, tolUs * 1.01));
}

// Verifies: REQ-F-DARS-004
// TEST-DARS-PHASE-003: Warning threshold differentiation for output at 48 kHz
TEST(PhaseToleranceTests, OutputWarningThreshold48k) {
    double sr = 48000.0;
    double warnUs = PhaseTolerance::output_warning_threshold_us(sr);
    double tolUs = PhaseTolerance::output_tolerance_us(sr);

    // Below warning threshold
    EXPECT_TRUE(PhaseTolerance::within_output(sr, warnUs * 0.98));

    // Between warning and tolerance (still within output tolerance)
    EXPECT_TRUE(PhaseTolerance::within_output(sr, warnUs * 1.02));
    EXPECT_TRUE(PhaseTolerance::within_output(sr, tolUs * 0.99));

    // Above tolerance
    EXPECT_FALSE(PhaseTolerance::within_output(sr, tolUs * 1.01));
}

// Simulate phase offsets resulting from small ppm drift over a frame at 48 kHz
// and verify output/input tolerances behave as expected.
TEST(PhaseToleranceTests, PhaseOffsetFromPpmDrift) {
    double sr = 48000.0;
    double frameUs = PhaseTolerance::frame_period_us(sr);
    // 10 ppm drift over one frame ~ 10e-6 * frame period
    double offset10ppmUs = frameUs * 10.0 / 1'000'000.0;
    EXPECT_TRUE(PhaseTolerance::within_output(sr, offset10ppmUs)); // 10ppm is 0.001% << 5%
    EXPECT_TRUE(PhaseTolerance::within_input(sr, offset10ppmUs));

    // 20% of frame (beyond output tolerance, within input tolerance)
    double offset20pctUs = frameUs * 0.20;
    EXPECT_FALSE(PhaseTolerance::within_output(sr, offset20pctUs));
    EXPECT_TRUE(PhaseTolerance::within_input(sr, offset20pctUs));

    // 30% of frame (beyond both output and input tolerances)
    double offset30pctUs = frameUs * 0.30;
    EXPECT_FALSE(PhaseTolerance::within_output(sr, offset30pctUs));
    EXPECT_FALSE(PhaseTolerance::within_input(sr, offset30pctUs));
}
