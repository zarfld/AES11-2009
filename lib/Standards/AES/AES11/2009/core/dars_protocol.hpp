// DARS Protocol Core Skeleton - DES-C-001
// Original implementation based on understanding of AES-11-2009 requirements.
// No copyrighted specification content is reproduced. Refer to AES-11-2009 for authoritative details.

#ifndef AES_AES11_2009_CORE_DARS_PROTOCOL_HPP
#define AES_AES11_2009_CORE_DARS_PROTOCOL_HPP

#include <cstdint>

namespace AES {
namespace AES11 {
namespace _2009 {
namespace core {

// Minimal state enumeration for initial test coverage (TEST-DARS-STATE-001)
enum class DARSState : uint8_t {
    Idle = 0,
    Acquire,
    Locked,
    Error
};

// Events that may drive state transitions (expanded later)
enum class DARSEvent : uint8_t {
    BeginAcquire, // trigger acquisition
    FrameValid,   // valid frame indicates possible lock progression
    FrameInvalid, // invalid frame - could degrade or error
};

class DARSProtocol {
public:
    DARSProtocol() : _state(DARSState::Idle) {}

    DARSState state() const { return _state; }

    // Request acquisition (Idle -> Acquire). Returns true if transition occurred.
    bool requestAcquire() {
        if (_state == DARSState::Idle) {
            _state = DARSState::Acquire;
            return true;
        }
        return false; // No transition if not Idle
    }

    // Process a protocol event. Minimal state machine:
    // Idle -> Acquire on BeginAcquire
    // Acquire -> Locked on FrameValid; -> Error on FrameInvalid
    // Locked -> Error on FrameInvalid
    // Error -> Acquire on BeginAcquire (attempt recovery)
    void handleEvent(DARSEvent ev) {
        switch (_state) {
        case DARSState::Idle:
            if (ev == DARSEvent::BeginAcquire) _state = DARSState::Acquire;
            break;
        case DARSState::Acquire:
            if (ev == DARSEvent::FrameValid) _state = DARSState::Locked;
            else if (ev == DARSEvent::FrameInvalid) _state = DARSState::Error;
            break;
        case DARSState::Locked:
            if (ev == DARSEvent::FrameInvalid) _state = DARSState::Error;
            break;
        case DARSState::Error:
            if (ev == DARSEvent::BeginAcquire) _state = DARSState::Acquire;
            break;
        }
    }

private:
    DARSState _state;
};

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES

#endif // AES_AES11_2009_CORE_DARS_PROTOCOL_HPP
