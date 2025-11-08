// Sample Rate Validation implementation - delegates to AES5 repo if integrated.
// Original logic based on understanding of AES5-2018 preferred sampling frequencies.
// No copyrighted text reproduced.

#include "sample_rate_validation.hpp"
#include <cmath>
#include <limits>

// NOTE: AES5 integration headers are intentionally not included here due to
// repository-relative includes in AES5 headers. We keep a clean fallback
// that validates known AES5 standard rates and ppm tolerance locally.

namespace AES {
namespace AES11 {
namespace _2009 {
namespace core {

bool SampleRateValidator::is_aes5_standard(uint32_t rateHz) {
    // Fallback minimal set if AES5 repository not available
    switch (rateHz) {
        case 32000u: case 44100u: case 48000u: case 96000u: case 192000u: return true;
        case 88200u: case 176400u: return true; // multiples of 44.1 kHz
        default: return false;
    }
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

SampleRateValidator::ValidationCategory SampleRateValidator::classify(uint32_t nominalHz, double measuredHz) {
    // PASS if AES5 standard base/multiple and within ±1 ppm
    // WARNING if AES5 standard and within ±10 ppm (but > 1 ppm)
    // FAIL otherwise (non-standard or >10 ppm)
    ValidationResult r = evaluate(nominalHz, measuredHz);
    if (!r.isStandard) {
        return ValidationCategory::Fail;
    }
    if (r.ppmError <= 1.0 + 1e-12) {
        return ValidationCategory::Pass;
    }
    if (r.ppmError <= 10.0 + 1e-12) {
        return ValidationCategory::Warning;
    }
    return ValidationCategory::Fail;
}

// NOTE: AES5 integration temporarily disabled in this compilation unit due to
// namespace collision and relative include issues. Adapter functions below
// provide stub behavior until a clean, out-of-namespace include strategy is
// implemented (see TODO: Fix AES5 submodule compile errors).

// AES5Adapter method implementations moved to separate translation unit to
// keep AES5 includes out of AES namespaces and avoid namespace pollution.

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES
