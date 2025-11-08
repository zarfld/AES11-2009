// Sample Rate Validation implementation - delegates to AES5 repo if integrated.
// Original logic based on understanding of AES5-2018 preferred sampling frequencies.
// No copyrighted text reproduced.

#include "sample_rate_validation.hpp"
#include <cmath>
#include <limits>

#ifdef AES5_INTEGRATION
#include "AES/AES5/2018/rates/standard_sampling_rates.hpp"
using AES::AES5::_2018::rates::StandardSamplingRates;
#endif

namespace AES {
namespace AES11 {
namespace _2009 {
namespace core {

bool SampleRateValidator::is_aes5_standard(uint32_t rateHz) {
#ifdef AES5_INTEGRATION
    return StandardSamplingRates::is_standard_rate(rateHz);
#else
    // Fallback minimal set if AES5 repository not available
    switch (rateHz) {
        case 32000u: case 44100u: case 48000u: case 96000u: case 192000u: return true;
        case 88200u: case 176400u: return true; // multiples of 44.1 kHz
        default: return false;
    }
#endif
}

static inline double ppm_error(double nominalHz, double measuredHz) {
    if (nominalHz <= 0.0) return std::numeric_limits<double>::infinity();
    double err = (measuredHz - nominalHz) / nominalHz;
    return std::abs(err) * 1'000'000.0; // convert fraction to ppm
}

SampleRateValidator::ValidationResult SampleRateValidator::evaluate(uint32_t nominalHz, double measuredHz) {
    ValidationResult r{};
    r.isStandard = is_aes5_standard(nominalHz);
    if (nominalHz == 0u) {
        r.ppmError = std::numeric_limits<double>::infinity();
        return r;
    }
    r.ppmError = ppm_error(static_cast<double>(nominalHz), measuredHz);
    return r;
}

bool SampleRateValidator::within_tolerance(uint32_t nominalHz, double measuredHz, double ppmTolerance) {
    if (nominalHz == 0u) return false;
    if (ppmTolerance < 0.0) return false;
    auto r = evaluate(nominalHz, measuredHz);
    return r.ppmError <= ppmTolerance + 1e-12;
}

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES
