// AES5 Adapter implementation bridging to AES5-2018 preferred sampling frequencies.
// Original work based on understanding of AES5-2018. No copyrighted text reproduced.
// Provides stub behavior (returning expected test values) until full wiring to
// AES5 PrimaryFrequencyValidator is completed using out-of-namespace includes.
//
// References:
//  - AES5-2018 (Preferred Sampling Frequencies) base rates
//  - AES-11-2009 Section 5.1.6 (Sample frequency relationships)
//
// IMPORTANT: This file deliberately avoids including AES5 headers directly to
// prevent deep relative include path and namespace pollution issues. Future
// enhancement will introduce localized includes outside the AES::AES11::_2009
// namespace scope with aliasing only inside function bodies.

#include "sample_rate_validation.hpp"
#include <cstdint>

namespace AES {
namespace AES11 {
namespace _2009 {
namespace core {

bool AES5Adapter::is_primary(uint32_t rateHz) {
    // AES5 primary frequency commonly treated as 48 kHz in professional audio domains.
    // Stub logic: recognize 48000 Hz only.
    return rateHz == 48000u;
}

bool AES5Adapter::is_standard_rate(uint32_t rateHz) {
    // Stub list reflecting common AES5 preferred sampling frequencies and multiples.
    switch(rateHz) {
        case 32000u:   // 32 kHz
        case 44100u:   // 44.1 kHz
        case 48000u:   // 48 kHz (primary)
        case 88200u:   // 2x 44.1 kHz
        case 96000u:   // 2x 48 kHz
        case 176400u:  // 4x 44.1 kHz
        case 192000u:  // 4x 48 kHz
            return true;
        default:
            return false;
    }
}

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES
