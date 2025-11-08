// AES3 Adapter - Protocol integration scaffolding
// Wraps AES3 preamble/TRP handling behind a thin interface with optional external integration.
// No hardware-specific code; optional compile-time integration guarded by AES3_INTEGRATION.

#ifndef AES_AES11_2009_CORE_AES3_ADAPTER_HPP
#define AES_AES11_2009_CORE_AES3_ADAPTER_HPP

#include <cstdint>
#include <cstddef>
#include <optional>

namespace AES {
namespace AES11 {
namespace _2009 {
namespace core {

struct TRPInfo {
    bool valid;
    // Timestamp is to be provided by caller from a timing HAL in real integration
    // Here we expose only a Boolean validity and byte offset to the TRP edge in the frame buffer if available
    std::optional<size_t> preamble_offset_bits;
};

class AES3Adapter {
public:
    // Detect if a raw AES3 subframe buffer appears to have a valid X/Y/Z preamble pattern.
    // This scaffolding uses a placeholder heuristic when AES3 external is unavailable.
    static bool detect_preamble(const uint8_t* frameBytes, size_t bitLength);

    // Extract TRP-related information from raw bytes; in full integration, delegate to AES3 repo utilities.
    static TRPInfo extract_trp(const uint8_t* frameBytes, size_t bitLength);
};

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES

#endif // AES_AES11_2009_CORE_AES3_ADAPTER_HPP
