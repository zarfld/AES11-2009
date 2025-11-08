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
    bool valid{false};
    // Timestamp would be provided externally (e.g., via injected timing interface)
    // We expose: validity flag, detected preamble type, and bit offset to first preamble slot.
    enum class PreambleType : uint8_t { Unknown=0, X=0xE2, Y=0xE4, Z=0xE8 };
    PreambleType type{PreambleType::Unknown};
    std::optional<size_t> preamble_offset_bits; // Bit index of first preamble slot (LSB first within pattern)
};

class AES3Adapter {
public:
    /**
     * @brief Detect if raw subframe bytes contain a valid AES3 preamble (X/Y/Z).
     *
     * When AES3_INTEGRATION is defined, delegates to external AES3 Part 3 PreambleGenerator logic.
     * Otherwise uses a conservative local pattern match on first byte.
     *
     * @param frameBytes Pointer to subframe/frame buffer (must contain at least preamble byte)
     * @param bitLength  Length of buffer expressed in bits (>= 8 required for preamble byte)
     * @return true if a recognizable preamble pattern is found
     */
    static bool detect_preamble(const uint8_t* frameBytes, size_t bitLength) noexcept;

    /**
     * @brief Extract Timing Reference Point (TRP) information based on preamble.
     *
     * Uses AES3 preamble semantics: X (first subframe), Y (second), Z (block start).
     * For AES11 DARS alignment we treat X or Z as potential timing reference boundaries per AES-11-2009 Section 3.3.
     *
     * @param frameBytes Pointer to subframe/frame buffer
     * @param bitLength  Buffer length in bits
     * @return TRPInfo structure with validity, preamble type and offset
     */
    static TRPInfo extract_trp(const uint8_t* frameBytes, size_t bitLength) noexcept;
};

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES

#endif // AES_AES11_2009_CORE_AES3_ADAPTER_HPP
