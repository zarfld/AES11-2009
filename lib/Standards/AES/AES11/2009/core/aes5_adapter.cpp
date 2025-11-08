// AES5 Adapter implementation bridging to AES5-2018 preferred sampling frequencies.
// Now wired to AES5 PrimaryFrequencyValidator when AES5_INTEGRATION is defined.
// Falls back to internal stub behavior if integration is disabled.
//
// References (no copyrighted text reproduced):
//  - AES5-2018 Section 5.1 (Primary sampling frequency)
//  - AES-11-2009 Section 5.1.6 (Sample frequency relationships)

#include "sample_rate_validation.hpp"
#include <cstdint>

#if defined(AES5_INTEGRATION) && defined(AES5_WIRE_OK)
// Include AES5 headers OUTSIDE the AES::AES11 namespace scope to avoid nesting pollution.
// Path uses target include directories added in root CMakeLists (external/AES5-2018/lib/Standards)
#include "AES/AES5/2018/core/frequency_validation/primary_frequency_validator.h"
#endif

namespace AES {
namespace AES11 {
namespace _2009 {
namespace core {

bool AES5Adapter::is_primary(uint32_t rateHz) {
#if defined(AES5_INTEGRATION) && defined(AES5_WIRE_OK)
    using AES::AES5::_2018::core::frequency_validation::PrimaryFrequencyValidator;
    return PrimaryFrequencyValidator::is_primary_frequency(rateHz);
#else
    return rateHz == 48000u; // fallback stub
#endif
}

bool AES5Adapter::is_standard_rate(uint32_t rateHz) {
#if defined(AES5_INTEGRATION) && defined(AES5_WIRE_OK)
    using namespace AES::AES5::_2018::core::frequency_validation;
    aes5_frequency_category_t category = AES5_CATEGORY_UNKNOWN;
    (void)PrimaryFrequencyValidator::validate_sampling_frequency(rateHz, &category);
    switch(category) {
        case AES5_CATEGORY_PRIMARY:
        case AES5_CATEGORY_OTHER:
        case AES5_CATEGORY_MULTIPLE_2X:
        case AES5_CATEGORY_MULTIPLE_4X:
        case AES5_CATEGORY_MULTIPLE_8X:
        case AES5_CATEGORY_LEGACY:
            return true; // treat all recognized categories as standard rates
        default:
            return false;
    }
#else
    switch(rateHz) {
        case 32000u: case 44100u: case 48000u: case 88200u: case 96000u: case 176400u: case 192000u:
            return true;
        default:
            return false;
    }
#endif
}

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES
