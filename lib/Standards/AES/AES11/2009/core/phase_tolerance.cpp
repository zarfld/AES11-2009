#include "phase_tolerance.hpp"
#include <cmath>

namespace AES {
namespace AES11 {
namespace _2009 {
namespace core {

double PhaseTolerance::frame_period_us(double sampleRateHz) {
    if (sampleRateHz <= 0.0) return 0.0;                // Invalid; caller should handle
    return 1'000'000.0 / sampleRateHz;                  // microseconds per frame
}

double PhaseTolerance::output_tolerance_us(double sampleRateHz) {
    return frame_period_us(sampleRateHz) * 0.05; // ±5% output
}

double PhaseTolerance::input_tolerance_us(double sampleRateHz) {
    return frame_period_us(sampleRateHz) * 0.25; // ±25% input
}

double PhaseTolerance::output_warning_threshold_us(double sampleRateHz) {
    return frame_period_us(sampleRateHz) * 0.045; // ±4.5% warning
}

bool PhaseTolerance::within_output(double sampleRateHz, double absPhaseOffsetUs) {
    if (sampleRateHz <= 0.0) return false;
    return absPhaseOffsetUs <= output_tolerance_us(sampleRateHz);
}

bool PhaseTolerance::within_input(double sampleRateHz, double absPhaseOffsetUs) {
    if (sampleRateHz <= 0.0) return false;
    return absPhaseOffsetUs <= input_tolerance_us(sampleRateHz);
}

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES
