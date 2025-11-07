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
    - REQ-F-HAL-001
    - REQ-F-HAL-002
    - REQ-F-HAL-003
    - REQ-F-HAL-004
    - REQ-F-HAL-005
    - REQ-F-HAL-006
    - REQ-F-HAL-007
---
# Interface Specifications

This document lists architectural interfaces. Hardware access occurs exclusively through HAL interfaces; the standards layer declares facades only.

## INT-001: Audio HAL Facade

- Provided by: Service Layer (platform adapters)
- Used by: ARC-C-001 Standards Core
- Operations (indicative):
  - send_audio_frame(frame)
  - receive_audio_frame(frame)
  - set_sample_rate(hz)
  - get_sample_rate()

## INT-002: Timing HAL Facade

- Provided by: Service Layer
- Used by: ARC-C-004 Synchronization Manager
- Operations (indicative):
  - get_time_ns()
  - get_sample_time()
  - get_timing_resolution_ns()

## INT-003: Sync HAL Facade

- Provided by: Service Layer
- Used by: ARC-C-004 Synchronization Manager
- Operations (indicative):
  - get_timing_reference(ref)
  - lock_to_reference(freq_hz)
  - get_lock_status(status)

## INT-004: Platform Capabilities Interface

- Provided by: Service Layer
- Used by: Standards Core / DARS modules
- Operations (indicative):
  - get_platform_capabilities(caps)

## INT-005: Conformance Harness API

- Provided by: ARC-C-006
- Used by: Test runners, measurement tools
- Purpose: Run AES-11 section 5/6 compliance suites, export results

Notes:

- Exact function signatures are specified in requirements and will be concretized during design (Phase 04).
- All interfaces avoid vendor/OS types; use plain C/C++ types.

Related ADRs: ADR-001, ADR-004
