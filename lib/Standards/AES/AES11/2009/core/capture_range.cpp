#include "capture_range.hpp"
#include <cmath>
#include <limits>

namespace AES {
namespace AES11 {
namespace _2009 {
namespace core {

bool CaptureRange::within_capture(double absPpmError, Grade grade) {
    switch (grade) {
    case Grade::Grade1:
        // REQ-F-DARS-003 (±2 ppm)
        return absPpmError <= 2.0;
    case Grade::Grade2:
        // REQ-F-DARS-003 (±50 ppm)
        return absPpmError <= 50.0;
    }
    return false;
}

double CaptureRange::ppm_error(double expectedHz, double measuredHz) {
    if (expectedHz == 0.0) return std::numeric_limits<double>::infinity(); // REQ-F-DARS-003 safeguard
    double diff = measuredHz - expectedHz;
    return std::abs(diff / expectedHz * 1'000'000.0);
}

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES
