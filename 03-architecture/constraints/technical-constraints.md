---
specType: guidance
title: Technical Constraints
author: "AI Assistant"
date: "2025-11-07"
---
## Technical Constraints

## Platform and Build

- Language: C / C++17 (standards layer)
- Build System: CMake ≥3.20
- Compilers: GCC 9+, Clang 10+, MSVC 19.20+

## Portability Constraints

- No vendor or OS-specific headers in AES::AES11::_2009
- All hardware/timing access via HAL facades
- External standards via repos (AES3-2009, AES5-2018)

## Real-Time Constraints

- Real-time paths use static/pool allocation only
- Bounded execution, no blocking, no dynamic allocation in RT
- Timing resolution validated at startup (≤100 ns resolution target)

## Security/Compliance

- Follow CERT C/C++ guidelines
- Input validation for all external data (frames, references)
- Logging avoids sensitive data; provides diagnostics for conformance

## Dependency Management

- Version pinning for external repos
- CI validation across platforms
- Namespace isolation enforced
