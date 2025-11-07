---
specType: architecture
standard: "42010"
phase: 03-architecture
version: "1.0.0"
author: "AI Assistant"
date: "2025-11-07"
status: draft
traceability:
  requirements:
    - REQ-F-DARS-001
    - REQ-F-DARS-002
    - REQ-F-SYNC-001
    - REQ-NF-PERF-001
---

# Architecture Description

This Architecture Description follows ISO/IEC/IEEE 42010:2011 and describes the software architecture for the AES-11-2009 (DARS) hardware-agnostic implementation.

- Standard alignment: ISO/IEC/IEEE 42010 (architecture), IEEE 1016 (design), ISO/IEC/IEEE 29148 (requirements)
- Compliance drivers: AES-11-2009 (primary), AES3-2009, AES5-2018 (supporting)
- Core principles: Hardware-agnostic standards layer, strict layering, real-time safety, complete standards compliance

## 1. Introduction

### 1.1 Purpose

Provide a shared architectural blueprint, views, and decisions to guide detailed design and implementation while ensuring conformance with the SyRS (02-requirements/system-requirements-specification.md).

### 1.2 Scope

Covers the standards-layer library implementing AES::AES11::_2009 (core/dars/sync/video/conformity), integration boundaries to external standards (AES3, AES5), and HAL interfaces provided by the service layer (no vendor code in standards layer).

### 1.3 Definitions, Acronyms, Abbreviations

See Section 9 of the SyRS (Glossary & References). Key abbreviations: DARS, TRP, HAL, ppm, ppb, PTP.

### 1.4 References

- SyRS-AES11-001 (System Requirements Specification)
- AES-11-2009 (DARS)
- AES3-2009 (Digital Audio Interface)
- AES5-2018 (Preferred Sampling Frequencies)
- ISO/IEC/IEEE 42010:2011, IEEE 1016-2009

### 1.5 Overview

This document defines stakeholders and concerns, viewpoints and views (with C4 diagrams), architecture decisions (ADRs), technology stack, constraints, traceability, and a brief architecture evaluation.

## 2. Stakeholders and Concerns

| Stakeholder | Key Concerns |
|-------------|--------------|
| Developers | Clear module boundaries, testability, portability, simple build |
| QA/V&V | Testability, conformance coverage, reproducibility |
| Integrators | HAL boundaries, external repository integration (AES3/AES5) |
| Security/Compliance | Standards conformance, no vendor/OS leakage into core |
| Product/PM | Time-to-market, maintainability, risk control |

## 3. Architecture Viewpoints

- Logical Viewpoint: components, responsibilities, dependencies
- Process Viewpoint: runtime interactions, state machines, timing
- Development Viewpoint: code structure, layers, modules
- Physical Viewpoint: deployment for dev/test, supported platforms
- Data Viewpoint: key data structures and flows

Modeling: Mermaid (C4-style), sequence diagrams, structured tables.

## 4. Architecture Views

### 4.1 Logical View (Component Overview)

Key components (standards layer only):

- ARC-C-001 Standards Core Library (AES::AES11::_2009::core)
- ARC-C-002 DARS Generator (AES::AES11::_2009::dars)
- ARC-C-003 DARS Validator (AES::AES11::_2009::dars)
- ARC-C-004 Synchronization Manager (AES::AES11::_2009::core)
- ARC-C-005 Video Sync Alignment (AES::AES11::_2009::video)
- ARC-C-006 Conformance Test Harness (AES::AES11::_2009::conformity)
- ARC-C-007 AES3 Integration Adapter (AES::AES3::_2009) – external repo usage
- ARC-C-008 AES5 Integration Adapter (AES::AES5::_2018) – external repo usage
- ARC-C-009 HAL Facades (interfaces only; provided/implemented in service layer)


### 4.2 Process View (Key Runtime Interactions)

- DARS frame creation, TRP extraction, lock acquisition and holdover
- Video-referenced alignment (TRP aligned to video sync)
- Jitter measurement and tolerance validation
- Conformance test orchestration (automated suites)

A detailed sequence is provided in diagrams/c4-model/level-3-component-standards-lib.md.

### 4.3 Development View (Structure)

 
```text
lib/Standards/
└── AES/AES11/2009/
    ├── core/
    ├── dars/
    ├── sync/
    ├── video/
    └── conformity/
```

External dependencies: AES3-2009, AES5-2018 (separate repos via CMake); no vendor APIs in standards layer. HAL adapters live outside standards layer.

### 4.4 Physical View (Deployment/Targets)

- Embedded: ARM Cortex-M7 (RTOS)
- Desktop: x86-64 (Linux, Windows, macOS)
- CI: Multi-platform runners, unit + conformance tests

### 4.5 Data View (Key Data Objects)

- DARS frame (via AES3 frame structure), TRP markers, channel status
- Timing references (video sync, GPS 1PPS), lock status, jitter statistics

## 5. Architecture Decisions (ADRs)

- ADR-001: Layered architecture with HAL boundaries
- ADR-002: DARS state machine design (acquire/lock/holdover)
- ADR-003: Timing and timestamp sources abstraction
- ADR-004: Memory strategy (static/pool vs dynamic, zero in RT paths)
- ADR-005: External standards reuse (AES3/AES5) and namespace isolation

See 03-architecture/decisions/.

## 6. Technology Stack

- Language: C / C++17 (headers only where possible in standards layer)
- Build: CMake ≥3.20, toolchains: GCC/Clang/MSVC
- Test: GoogleTest, Unity, pytest (harness)
- Analysis: clang-tidy, cppcheck; coverage: gcov/lcov

## 7. Constraints and Assumptions

- No vendor/OS-specific headers in AES::AES11::_2009
- Real-time paths: static allocation, bounded execution, no blocking
- Strict reuse of AES3/AES5 external repos (no duplication)
- HAL-only boundary for hardware/timing access

## 8. Traceability to Requirements

| Architecture Element | Requirements |
|----------------------|--------------|
| ARC-C-001 Standards Core | REQ-F-DARS-001, REQ-F-DARS-002, REQ-F-DARS-003 |
| ARC-C-002 DARS Generator | REQ-F-DARS-005, REQ-F-DARS-006 |
| ARC-C-004 Sync Manager | REQ-F-SYNC-001..004, REQ-F-DARS-004 |
| ARC-C-006 Conformance | REQ-F-CONFORM-001..008 |
| HAL Facades | REQ-F-HAL-001..007, REQ-NF-PORT-001..003 |
| Performance Architecture | REQ-NF-PERF-001..005 |
| Reliability Architecture | REQ-NF-REL-001..006 |
| Security Architecture | REQ-NF-SEC-001..004 |

## 9. Architecture Evaluation (Initial)

- Performance: Latency budget <1 ms end-to-end (REQ-NF-PERF-001)
- Reliability: MTBF ≥10,000 h, availability ≥99.9% (REQ-NF-REL-001/002)
- Security: CERT C/C++ baseline (REQ-NF-SEC-003); input validation (REQ-NF-SEC-001)
- Risks: Platform HAL variability (mitigated by validation tool), timing source accuracy (mitigated by abstraction and calibration)

## 10. References to Diagrams and ADRs

- C4 diagrams under diagrams/c4-model/
- ADRs under decisions/
