#include <gtest/gtest.h>
#include "../../lib/Standards/AES/AES11/2009/sync/gps_reference_sync.hpp"

using AES::AES11::_2009::sync::GPSReferenceSync;

// Verifies: REQ-F-DARS-006
// TEST-GPS-REF-001: Exact alignment (0.2 µs offset) within ±1 µs tolerance
TEST(GPSReferenceSyncTests, AlignmentWithinTolerance) {
    uint64_t pps = 1'000'000'000ULL; // 1 second in ns
    uint64_t trp = pps + 200;        // 0.2 µs late
    double offset = GPSReferenceSync::phase_offset_us(trp, pps);
    EXPECT_NEAR(offset, 0.2, 0.0001);
    EXPECT_TRUE(GPSReferenceSync::within_alignment(trp, pps));
}

// Verifies: REQ-F-DARS-006
// TEST-GPS-REF-002: Offset exceeding default ±1 µs tolerance should fail
TEST(GPSReferenceSyncTests, AlignmentExceedsTolerance) {
    uint64_t pps = 2'000'000'000ULL;
    uint64_t trp = pps + 1'500; // 1.5 µs offset
    double offset = GPSReferenceSync::phase_offset_us(trp, pps);
    EXPECT_NEAR(offset, 1.5, 0.0001);
    EXPECT_FALSE(GPSReferenceSync::within_alignment(trp, pps));
}

// Verifies: REQ-F-DARS-006
// TEST-GPS-REF-003: Custom tolerance (±2 µs) accepts 1.5 µs offset
TEST(GPSReferenceSyncTests, CustomToleranceAcceptance) {
    uint64_t pps = 3'000'000'000ULL;
    uint64_t trp = pps - 1'500; // early by 1.5 µs
    EXPECT_TRUE(GPSReferenceSync::within_alignment(trp, pps, 2.0));
    EXPECT_FALSE(GPSReferenceSync::within_alignment(trp, pps)); // default 1.0 µs
}
