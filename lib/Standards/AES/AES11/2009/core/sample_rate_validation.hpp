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
    // Returns true if the provided rate is an AES5-2018 standard base rate
    // supported by AES-11 DARS (base family: 32k, 44.1k, 48k and multiples
    // up to 192 kHz). This initial implementation validates common base
    // rates required by the current design and tests.
    static bool is_aes5_standard(uint32_t rateHz);

    // Returns true if measuredHz is within ±ppmTolerance parts-per-million
    // of nominalHz. Useful for initial accuracy checks (REQ-F-DARS-008).
    static bool within_tolerance(uint32_t nominalHz, double measuredHz, double ppmTolerance);
};

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES

#endif // AES_AES11_2009_CORE_SAMPLE_RATE_VALIDATION_HPP
