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
