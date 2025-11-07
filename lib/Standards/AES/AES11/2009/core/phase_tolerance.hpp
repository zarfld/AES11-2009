/*
 * This file implements protocol functionality based on understanding of:
 * - AES-11-2009 (Digital Audio Reference Signals) - Copyright AES
 * - AES3-2009 (Digital Audio Interface) - Copyright AES
 * No copyrighted content from these specifications is reproduced.
 * Implementation is original work achieving compliance through understanding
 * of specification requirements.
 * For authoritative requirements, refer to the original specifications.
 */

#ifndef AES_AES11_2009_CORE_PHASE_TOLERANCE_HPP
#define AES_AES11_2009_CORE_PHASE_TOLERANCE_HPP

#include <cstdint>

namespace AES {
namespace AES11 {
namespace _2009 {
namespace core {

/**
 * @brief Phase tolerance evaluator for Timing Reference Point (TRP)
 *
 * Provides helpers to compute AES3 frame period and to evaluate whether
 * measured TRP phase offsets for inputs/outputs are within tolerance bands.
 *
 * @note Implements REQ-F-DARS-004 (Phase Relationship Tolerances)
 *       and relates to REQ-F-DARS-002 (frequency/phase measurement context).
 * @see AES-11-2009 Section 5.3.1 (TRP) and Table 2 (phase tolerances)
 */
class PhaseTolerance {
public:
    /**
     * @brief Compute AES3 frame period in microseconds from sampling rate (Hz)
     */
    static double frame_period_us(double sampleRateHz);

    /**
     * @brief Output phase tolerance (±5% of frame period) in microseconds
     */
    static double output_tolerance_us(double sampleRateHz);

    /**
     * @brief Input phase tolerance (±25% of frame period) in microseconds
     */
    static double input_tolerance_us(double sampleRateHz);

    /**
     * @brief Warning threshold for output phase (±4.5% of frame period), µs
     */
    static double output_warning_threshold_us(double sampleRateHz);

    /**
     * @brief Returns true if absolute phase offset (µs) is within output tolerance (±5%)
     */
    static bool within_output(double sampleRateHz, double absPhaseOffsetUs);

    /**
     * @brief Returns true if absolute phase offset (µs) is within input tolerance (±25%)
     */
    static bool within_input(double sampleRateHz, double absPhaseOffsetUs);
};

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES

#endif // AES_AES11_2009_CORE_PHASE_TOLERANCE_HPP
