#include "gps_reference_sync.hpp"
#include <cmath>

namespace AES {
namespace AES11 {
namespace _2009 {
namespace sync {

double GPSReferenceSync::phase_offset_us(uint64_t trpTimeNs, uint64_t ppsTimeNs) {
    if (trpTimeNs > ppsTimeNs) {
        return static_cast<double>(trpTimeNs - ppsTimeNs) / 1000.0;
    }
    return static_cast<double>(ppsTimeNs - trpTimeNs) / 1000.0;
}

bool GPSReferenceSync::within_alignment(uint64_t trpTimeNs, uint64_t ppsTimeNs, double tolerance_us) {
    return phase_offset_us(trpTimeNs, ppsTimeNs) <= tolerance_us; // REQ-F-DARS-006 ±1 µs default
}

} // namespace sync
} // namespace _2009
} // namespace AES11
} // namespace AES
