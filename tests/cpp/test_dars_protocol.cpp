#include <gtest/gtest.h>
#include "../../lib/Standards/AES/AES11/2009/core/dars_protocol.hpp"

using AES::AES11::_2009::core::DARSProtocol;
using AES::AES11::_2009::core::DARSState;
using AES::AES11::_2009::core::DARSEvent;

// Verifies: REQ-F-SYNC-001 (DARS-referenced synchronization state transitions)
// TEST-DARS-STATE-001: Validate initial transition Idle -> Acquire via requestAcquire()
TEST(DARSProtocolTests, IdleToAcquireTransition) {
    DARSProtocol proto;
    ASSERT_EQ(proto.state(), DARSState::Idle);
    bool transitioned = proto.requestAcquire();
    EXPECT_TRUE(transitioned);
    EXPECT_EQ(proto.state(), DARSState::Acquire);
}

// Extended check: Acquire -> Locked when FrameValid event received
TEST(DARSProtocolTests, AcquireToLockedOnFrameValid) {
    DARSProtocol proto;
    proto.requestAcquire();
    ASSERT_EQ(proto.state(), DARSState::Acquire);
    proto.handleEvent(DARSEvent::FrameValid);
    EXPECT_EQ(proto.state(), DARSState::Locked);
}

// Negative path: requestAcquire ignored when already acquiring or locked
TEST(DARSProtocolTests, RequestAcquireIgnoredIfNotIdle) {
    DARSProtocol proto;
    proto.requestAcquire();
    EXPECT_FALSE(proto.requestAcquire()); // already Acquire
    proto.handleEvent(DARSEvent::FrameValid);
    EXPECT_FALSE(proto.requestAcquire()); // locked
}

// Edge Case: Invalid transitions should not change from Idle when FrameValid without BeginAcquire.
TEST(DARSProtocolTests, FrameValidIgnoredInIdle) {
    DARSProtocol proto;
    ASSERT_EQ(proto.state(), DARSState::Idle);
    proto.handleEvent(DARSEvent::FrameValid);
    EXPECT_EQ(proto.state(), DARSState::Idle);
}

// Event: BeginAcquire should transition Idle -> Acquire via handleEvent
TEST(DARSProtocolTests, BeginAcquireEventTransitionsToAcquire) {
    DARSProtocol proto;
    ASSERT_EQ(proto.state(), DARSState::Idle);
    proto.handleEvent(DARSEvent::BeginAcquire);
    EXPECT_EQ(proto.state(), DARSState::Acquire);
}

// In Acquire, FrameInvalid should not force Error; it should keep acquiring (current behavior)
TEST(DARSProtocolTests, FrameInvalidKeepsAcquireState) {
    DARSProtocol proto;
    ASSERT_TRUE(proto.requestAcquire());
    ASSERT_EQ(proto.state(), DARSState::Acquire);
    proto.handleEvent(DARSEvent::FrameInvalid);
    EXPECT_EQ(proto.state(), DARSState::Acquire);
}

