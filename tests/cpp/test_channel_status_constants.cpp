// Traceability Header
// Module: tests/cpp/test_channel_status_constants.cpp
// Phase: 05-implementation
// Traceability:
//   Design: DES-C-001
//   Requirements: REQ-F-CS-ANNEXA-FLAGS, REQ-F-CS-ANNEXA-LEN
//   Test ID: TEST-UNIT-ChannelStatusConstants
// Notes: Failing test first (TDD RED) to enforce mask uniqueness & index range consistency.

#include <gtest/gtest.h>
#include <set>
#include <cstdint>

#include "../../lib/Standards/AES/AES11/2009/core/channel_status_constants.hpp"

using namespace AES::AES11::_2009::core::constants;

// Verify that all defined flag masks occupy distinct bit positions (no overlap)
TEST(ChannelStatusConstantsTests, MasksAreUniqueAndNonOverlapping) {
    uint8_t combined = 0x00;
    std::set<uint8_t> seen;
    const uint8_t masks[] = { kUtcValidMask, kLeapPendingMask, kDstMask, kNonAudioMask, kAlignmentMarkerMask };
    for (auto m : masks) {
        // Each mask should be power-of-two (single bit) and not already seen
        EXPECT_NE(m, 0u) << "Mask must be non-zero";
        EXPECT_TRUE((m & (m - 1)) == 0) << "Mask must represent a single bit";
        EXPECT_TRUE(seen.find(m) == seen.end()) << "Duplicate mask detected";
        EXPECT_EQ((combined & m), 0u) << "Bit overlap detected";
        seen.insert(m);
        combined |= m;
    }
}

// Validate indices do not collide and are within the declared minimum block length
TEST(ChannelStatusConstantsTests, IndicesWithinRangeAndDistinct) {
    EXPECT_LT(kByte4Index, kChannelStatusMinBytes);
    EXPECT_LT(kUtcFlagsIndex, kChannelStatusMinBytes);
    EXPECT_LT(kTzOffsetLoIndex, kChannelStatusMinBytes);
    EXPECT_LT(kTzOffsetHiIndex, kChannelStatusMinBytes);
    EXPECT_LT(kDateTimeStartIndex, kChannelStatusMinBytes);
    EXPECT_LT(kDateTimeEndIndex, kChannelStatusMinBytes);
    // Distinctness (simple set check)
    std::set<size_t> indices{ kByte4Index, kUtcFlagsIndex, kTzOffsetLoIndex, kTzOffsetHiIndex };
    EXPECT_EQ(indices.size(), 4u) << "Unexpected index collision among core indices";
    // Date/time region must not overlap UTC flags or timezone offset bytes
    for (size_t i = kDateTimeStartIndex; i <= kDateTimeEndIndex; ++i) {
        EXPECT_TRUE(i != kUtcFlagsIndex) << "Date/time index overlaps UTC flags index";
        EXPECT_TRUE(i != kTzOffsetLoIndex) << "Date/time index overlaps TZ low byte";
        EXPECT_TRUE(i != kTzOffsetHiIndex) << "Date/time index overlaps TZ high byte";
    }
    EXPECT_LE(kDateTimeStartIndex, kDateTimeEndIndex);
}
