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

// Verifies: REQ-F-SYNC-001
// TEST-DARS-STATE-002: Acquire -> Locked when FrameValid event received
TEST(DARSProtocolTests, AcquireToLockedOnFrameValid) {
    DARSProtocol proto;
    proto.requestAcquire();
    ASSERT_EQ(proto.state(), DARSState::Acquire);
    proto.handleEvent(DARSEvent::FrameValid);
    EXPECT_EQ(proto.state(), DARSState::Locked);
}

// Verifies: REQ-F-SYNC-001
// TEST-DARS-STATE-003: requestAcquire ignored when already acquiring or locked
TEST(DARSProtocolTests, RequestAcquireIgnoredIfNotIdle) {
    DARSProtocol proto;
    proto.requestAcquire();
    EXPECT_FALSE(proto.requestAcquire()); // already Acquire
    proto.handleEvent(DARSEvent::FrameValid);
    EXPECT_FALSE(proto.requestAcquire()); // locked
}

// Verifies: REQ-F-SYNC-001
// TEST-DARS-STATE-004: FrameValid ignored in Idle (no BeginAcquire)
TEST(DARSProtocolTests, FrameValidIgnoredInIdle) {
    DARSProtocol proto;
    ASSERT_EQ(proto.state(), DARSState::Idle);
    proto.handleEvent(DARSEvent::FrameValid);
    EXPECT_EQ(proto.state(), DARSState::Idle);
}

// Verifies: REQ-F-SYNC-001
// TEST-DARS-STATE-005: BeginAcquire transitions Idle -> Acquire via handleEvent
TEST(DARSProtocolTests, BeginAcquireEventTransitionsToAcquire) {
    DARSProtocol proto;
    ASSERT_EQ(proto.state(), DARSState::Idle);
    proto.handleEvent(DARSEvent::BeginAcquire);
    EXPECT_EQ(proto.state(), DARSState::Acquire);
}

// Verifies: REQ-F-SYNC-001
// TEST-DARS-STATE-006: FrameInvalid keeps Acquire state (no error)
TEST(DARSProtocolTests, FrameInvalidKeepsAcquireState) {
    DARSProtocol proto;
    ASSERT_TRUE(proto.requestAcquire());
    ASSERT_EQ(proto.state(), DARSState::Acquire);
    proto.handleEvent(DARSEvent::FrameInvalid);
    EXPECT_EQ(proto.state(), DARSState::Acquire);
}

