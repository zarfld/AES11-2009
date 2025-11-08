#include "sample_rate_validation.hpp"

#include <cmath>
#include <limits>

namespace AES {
namespace AES11 {
namespace _2009 {
namespace core {

bool SampleRateValidator::is_aes5_standard(uint32_t rateHz) {
    // TODO: Replace with external AES5-2018 repository lookup (StandardSamplingRates)
    // Base AES5 rates commonly used in professional audio systems.
    switch (rateHz) {
        case 32000u:
        case 44100u:
        case 48000u:
        case 88200u:
        case 96000u:
        case 176400u:
        case 192000u:
            return true;
        default:
            return false;
    }
}

static inline double ppm_error(double nominalHz, double measuredHz) {
    if (nominalHz <= 0.0) return std::numeric_limits<double>::infinity();
    double err = (measuredHz - nominalHz) / nominalHz;
    return std::abs(err) * 1'000'000.0; // convert fraction to ppm
}

bool SampleRateValidator::within_tolerance(uint32_t nominalHz, double measuredHz, double ppmTolerance) {
    if (nominalHz == 0u) return false;
    if (ppmTolerance < 0.0) return false;
    return ppm_error(static_cast<double>(nominalHz), measuredHz) <= ppmTolerance + 1e-12;
}

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES
