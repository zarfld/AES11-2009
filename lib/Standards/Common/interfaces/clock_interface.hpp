// Hardware-agnostic clock interface for standards layer (Common namespace)
// Provides abstraction used by AES::AES11::_2009 components.

#ifndef COMMON_INTERFACES_CLOCK_INTERFACE_HPP
#define COMMON_INTERFACES_CLOCK_INTERFACE_HPP

#include <cstdint>

namespace Common {
namespace interfaces {

class ClockInterface {
public:
    virtual ~ClockInterface() = default;
    // Return a monotonically increasing tick counter (implementation-defined granularity)
    virtual uint64_t get_tick() = 0;
    // Return current time in nanoseconds from arbitrary but stable epoch
    virtual uint64_t get_time_ns() = 0;
};

} // namespace interfaces
} // namespace Common

#endif // COMMON_INTERFACES_CLOCK_INTERFACE_HPP
