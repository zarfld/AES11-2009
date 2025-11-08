/*
Module: lib/Standards/Common/testing/fault_injection.hpp
Phase: 05-implementation
Traceability:
  Design: DES-C-004 (Test hooks), DES-C-003 (Timing Window Processor - testability)
  Requirements: PHASE05-RELIABILITY-HOOKS, REQ-F-CS-ANNEXA-DT, REQ-F-CS-ANNEXA-LEN
  Tests: TEST-UNIT-FaultInjection
Notes: Fault injection utilities are compiled only when AES11_FAULT_INJECTION is defined.
*/
#pragma once

#include <string>
#include <unordered_set>
#include <mutex>

namespace Common {
namespace testing {

#ifdef AES11_FAULT_INJECTION
class FaultInjection {
public:
    // Enable a named fault point (e.g., "ChannelStatusUtils.set_datetime_info.fail")
    static void enable(const std::string& name) {
        std::lock_guard<std::mutex> lock(mtx());
        enabled().insert(name);
    }

    static void disable(const std::string& name) {
        std::lock_guard<std::mutex> lock(mtx());
        enabled().erase(name);
    }

    static void clear() {
        std::lock_guard<std::mutex> lock(mtx());
        enabled().clear();
    }

    static bool is_enabled(const std::string& name) {
        std::lock_guard<std::mutex> lock(mtx());
        return enabled().find(name) != enabled().end();
    }

private:
    static std::unordered_set<std::string>& enabled() {
        static std::unordered_set<std::string> s;
        return s;
    }
    static std::mutex& mtx() {
        static std::mutex m;
        return m;
    }
};
#else
class FaultInjection {
public:
    static void enable(const std::string&) {}
    static void disable(const std::string&) {}
    static void clear() {}
    static bool is_enabled(const std::string&) { return false; }
};
#endif

} // namespace testing
} // namespace Common
