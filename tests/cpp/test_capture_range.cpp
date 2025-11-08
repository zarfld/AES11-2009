#include <gtest/gtest.h>
#include "../../lib/Standards/AES/AES11/2009/core/capture_range.hpp"

using AES::AES11::_2009::core::CaptureRange;
using Grade = AES::AES11::_2009::core::CaptureRange::Grade;

// Verifies: REQ-F-DARS-003, REQ-F-DARS-002
// TEST-DARS-CAPTURE-001: Grade1 capture accepts error just below 2ppm and rejects just above
TEST(CaptureRangeTests, Grade1Boundary) {
    EXPECT_TRUE(CaptureRange::within_capture(1.999, Grade::Grade1));
    EXPECT_FALSE(CaptureRange::within_capture(2.001, Grade::Grade1));
}

// Verifies: REQ-F-DARS-003
// TEST-DARS-CAPTURE-002: Grade2 capture accepts up to 50ppm and rejects above
TEST(CaptureRangeTests, Grade2Boundary) {
    EXPECT_TRUE(CaptureRange::within_capture(49.9, Grade::Grade2));
    EXPECT_FALSE(CaptureRange::within_capture(50.1, Grade::Grade2));
}

// Verifies: REQ-F-DARS-003, REQ-F-DARS-002
// TEST-DARS-CAPTURE-003: ppm_error computes absolute difference correctly
TEST(CaptureRangeTests, PpmErrorComputation) {
    double expected = 48000.0;
    // 1000 ppm = 0.001 * expected = 48.0 Hz difference -> measured = 48000 + 48 = 48048.0
    double measured = 48048.0; // +1000 ppm (48/48000 * 1e6 = 1000)
    double ppm = CaptureRange::ppm_error(expected, measured);
    EXPECT_NEAR(ppm, 1000.0, 0.5);
}

// Grade timing precision simulation (REQ-F-DARS-002 / upcoming tests task)
// Simulate frequency drift and classify using capture thresholds for Grade1/Grade2.
TEST(CaptureRangeTests, GradeTimingPrecisionSimulation) {
    double expected = 48000.0;
    // Grade1 boundary just inside tolerance
    double measured_g1_ok = expected * (1.0 + 1.5 / 1'000'000.0); // +1.5 ppm
    double ppm_g1_ok = CaptureRange::ppm_error(expected, measured_g1_ok);
    EXPECT_TRUE(CaptureRange::within_capture(ppm_g1_ok, Grade::Grade1));

    // Grade1 violation just outside capture range
    double measured_g1_bad = expected * (1.0 + 2.5 / 1'000'000.0); // +2.5 ppm
    double ppm_g1_bad = CaptureRange::ppm_error(expected, measured_g1_bad);
    EXPECT_FALSE(CaptureRange::within_capture(ppm_g1_bad, Grade::Grade1));

    // Grade2 boundary inside tolerance
    double measured_g2_ok = expected * (1.0 + 45.0 / 1'000'000.0); // +45 ppm
    double ppm_g2_ok = CaptureRange::ppm_error(expected, measured_g2_ok);
    EXPECT_TRUE(CaptureRange::within_capture(ppm_g2_ok, Grade::Grade2));

    // Grade2 violation
    double measured_g2_bad = expected * (1.0 + 55.0 / 1'000'000.0); // +55 ppm
    double ppm_g2_bad = CaptureRange::ppm_error(expected, measured_g2_bad);
    EXPECT_FALSE(CaptureRange::within_capture(ppm_g2_bad, Grade::Grade2));
}
