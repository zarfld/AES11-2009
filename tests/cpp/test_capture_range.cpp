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
