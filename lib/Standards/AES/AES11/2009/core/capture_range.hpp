/*
 * This file implements protocol functionality based on understanding of:
 * - AES-11-2009 (Digital Audio Reference Signals) - Copyright AES
 * - AES3-2009 (Digital Audio Interface) - Copyright AES
 * - AES5-2018 (Preferred Sampling Frequencies) - Copyright AES
 * No copyrighted content from these specifications is reproduced.
 * Implementation is original work achieving compliance through understanding
 * of specification requirements.
 * For authoritative requirements, refer to the original specifications.
 */

#ifndef AES_AES11_2009_CORE_CAPTURE_RANGE_HPP
#define AES_AES11_2009_CORE_CAPTURE_RANGE_HPP

#include <cstdint>

namespace AES {
namespace AES11 {
namespace _2009 {
namespace core {

/**
 * @brief Capture range evaluator for DARS frequency acquisition
 *
 * Provides helpers to determine whether measured frequency error is within
 * the capture range for Grade 1 or Grade 2 systems.
 *
 * @note Implements aspects of REQ-F-DARS-003 (Capture Range Support)
 *       and relates to REQ-F-DARS-002 (Grade 1/2 Frequency Accuracy).
 * @see AES-11-2009 Section 5.2 (capture/hold ranges and grades)
 */
class CaptureRange {
public:
    enum class Grade : uint8_t {
        Grade1, // Professional (tight tolerance)
        Grade2  // Broadcast (wider tolerance)
    };

    // Returns true if absolute ppm error is within capture range for grade.
    // Grade 1: ±2 ppm (capture)
    // Grade 2: ±50 ppm (capture)
    static bool within_capture(double absPpmError, Grade grade);

    // Compute absolute ppm error from expected/measured Hz.
    static double ppm_error(double expectedHz, double measuredHz);
};

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES

#endif // AES_AES11_2009_CORE_CAPTURE_RANGE_HPP
