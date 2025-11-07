/*
 * Original implementation based on understanding of AES-11-2009 Section 4.2.4
 * (GPS-referenced synchronization). No copyrighted text is reproduced.
 */

#ifndef AES_AES11_2009_SYNC_GPS_REFERENCE_SYNC_HPP
#define AES_AES11_2009_SYNC_GPS_REFERENCE_SYNC_HPP

#include <cstdint>

namespace AES {
namespace AES11 {
namespace _2009 {
namespace sync {

/**
 * @brief GPS-referenced synchronization helpers (1PPS alignment)
 *
 * Provides utilities to evaluate phase alignment of DARS Timing Reference
 * Point (TRP) against GPS 1PPS rising edge and simple predicates for
 * ±1 µs alignment goals.
 *
 * @note Implements aspects of REQ-F-DARS-006 (GPS-Referenced Synchronization).
 */
class GPSReferenceSync {
public:
    /**
     * @brief Compute absolute phase offset between TRP and 1PPS in microseconds
     * @param trpTimeNs Timestamp of DARS TRP in nanoseconds
     * @param ppsTimeNs Timestamp of GPS 1PPS rising edge in nanoseconds
     * @return |trpTimeNs - ppsTimeNs| converted to microseconds (double)
     */
    static double phase_offset_us(uint64_t trpTimeNs, uint64_t ppsTimeNs);

    /**
     * @brief Predicate: within ±tolerance_us alignment between TRP and 1PPS
     * Defaults to ±1 µs per requirement.
     */
    static bool within_alignment(uint64_t trpTimeNs, uint64_t ppsTimeNs, double tolerance_us = 1.0);
};

} // namespace sync
} // namespace _2009
} // namespace AES11
} // namespace AES

#endif // AES_AES11_2009_SYNC_GPS_REFERENCE_SYNC_HPP
