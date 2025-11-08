// Sample Rate Validation Utilities - REQ-F-DARS-008 (AES5 Compliance)
// Original implementation based on understanding of AES-11-2009 (Section 5.1.6)
// and AES5-2018 preferred sampling frequencies. No copyrighted content is reproduced.
// This utility will later be wired to the external AES5-2018 repository for authoritative
// rate definitions; meanwhile, it validates against well-known AES5 base rates to satisfy
// TDD and integration, avoiding hardcoded usage at call sites.

#ifndef AES_AES11_2009_CORE_SAMPLE_RATE_VALIDATION_HPP
#define AES_AES11_2009_CORE_SAMPLE_RATE_VALIDATION_HPP

#include <cstdint>

namespace AES {
namespace AES11 {
namespace _2009 {
namespace core {

class SampleRateValidator {
public:
    struct ValidationResult {
        bool isStandard {false};
        double ppmError {0.0};
    };

    // Classification categories for REQ-F-DARS-008 enhanced validation
    enum class ValidationCategory {
        Pass,      // Standard rate and |ppm| <= 1
        Warning,   // Standard rate and |ppm| <= 10 (but > 1)
        Fail       // Non-standard rate or |ppm| > 10
    };

    // Returns true if the provided rate is an AES5-2018 standard base or multiple (via AES5 repo when available)
    static bool is_aes5_standard(uint32_t rateHz);

    // Compute ppm error and return structured result (without tolerance decision)
    static ValidationResult evaluate(uint32_t nominalHz, double measuredHz);

    // Returns true if measuredHz is within ±ppmTolerance parts-per-million of nominalHz.
    static bool within_tolerance(uint32_t nominalHz, double measuredHz, double ppmTolerance);

    // Determines PASS/WARNING/FAIL classification based on ppm thresholds
    static ValidationCategory classify(uint32_t nominalHz, double measuredHz);
};

// Thin wrapper adapter around AES5 PrimaryFrequencyValidator (if integrated) to avoid
// exposing AES5 headers to callers. Provides primary and standard rate checks.
class AES5Adapter {
public:
    // Returns true if 48 kHz is recognized as primary frequency (AES5 Section 5.1)
    static bool is_primary(uint32_t rateHz);
    // Returns true if rateHz is one of the recognized AES5 standard or multiple rates.
    static bool is_standard_rate(uint32_t rateHz);
};

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES

#endif // AES_AES11_2009_CORE_SAMPLE_RATE_VALIDATION_HPP
