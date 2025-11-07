# DES-I-002 Timing Source Interface

ID: DES-I-002  
Title: Timing Source Interface (hardware-agnostic)  
Spec Type: design (non-governed)  
Status: draft  
Owners: standards@project

## Purpose

Provide a minimal, hardware-agnostic interface that exposes precise timing primitives required by the AES-11-2009 DARS protocol and the Synchronization Manager. This interface is injected (dependency inversion) into standards-layer components and MUST NOT contain OS or vendor specifics.

Notes (standards context):

- Timing reference point alignment and capture precision must satisfy AES-11-2009 requirements (see Sections 3.3 and 5.x by number only; do not reproduce text).
- Supported sample rates align with AES5 preferred frequencies.

## Responsibilities

- Expose stable, monotonic time in nanoseconds suitable for ppm calculations and jitter windows.
- Provide an ever-increasing sample counter aligned to the active sample rate.
- Report the nominal sample rate and any detected deviations needed for Grade 1/2 determination.
- Produce atomic snapshots of timing state for consistent computations across frames.

Out of scope:

- Any direct hardware/driver calls, OS timing facilities, or blocking operations.

## Interface contract

Conceptual C++-like signature (language-agnostic design notation):

- get_sample_time_ns() -> uint64
  - Returns a monotonic timebase in nanoseconds suitable for interval error computation.

- get_sample_counter() -> uint64
  - Returns the total samples elapsed for the active clock domain (wraparound > 2^63 recommended).

- get_sample_rate_hz() -> uint32
  - Returns the nominal sample rate (must be one of AES5 values e.g., 32000, 44100, 48000, 96000).

- get_timing_snapshot() -> TimingSnapshot
  - Atomically returns { time_ns: uint64, sample_counter: uint64, sample_rate_hz: uint32 } captured as a coherent set.

- get_capabilities() -> Capabilities
  - Bitmask structure for optional features (e.g., hardware timestamp support, PPS discipline, PTP correlation).

### Data types

- TimingSnapshot
  - time_ns: uint64 (monotonic)
  - sample_counter: uint64
  - sample_rate_hz: uint32 (AES5-compliant)

- Capabilities (bitmask)
  - HW_TIMESTAMP: 0x01
  - PPS_DISCIPLINE: 0x02
  - PTP_CORRELATED: 0x04

### Preconditions

- Interface instance is initialized and stable before first use by DES-C-001 and DES-C-002.
- Sample rate reported remains constant unless explicitly reconfigured via a higher-level control path.

### Postconditions

- time_ns is strictly monotonic (never decreases).
- sample_counter is non-decreasing and advances by 1 per audio sample at the reported rate.
- get_timing_snapshot returns values that were sampled atomically (no torn reads across fields).

### Error handling

Return codes (language-agnostic):

- OK = 0
- ERR_NOT_READY = -1 (provider not yet initialized or locked)
- ERR_UNSUPPORTED_RATE = -2 (non-AES5 rate)
- ERR_CAPABILITY_REQUIRED = -3 (requested feature not present)

All functions must be non-throwing in the hot path. If the host language uses exceptions, they MUST NOT be used in time-critical calls.

## Concurrency and performance

- All operations are lock-free and O(1) in the steady-state hot path.
- No dynamic memory allocation in any timing query method.
- get_timing_snapshot is preferred to multiple separate calls to avoid TOCTOU and reduce jitter.
- Target execution budget per call: << 50 µs (REQ-NF-PERF-001 context; exact budget validated in tests for the target environment).

## Invariants and validation

- sample_rate_hz ∈ AES5 set.
- For two consecutive snapshots A then B: B.time_ns ≥ A.time_ns and B.sample_counter ≥ A.sample_counter.
- Derived interval error over windows computed by DES-D-002 must remain stable within configured tolerances for Grade 1/2 evaluation.

## Test hooks and observability

- Provide a pure software mock implementing deterministic sequences for time_ns and sample_counter to allow repeatable tests.
- Optional diagnostic callback to record snapshot latencies (disabled by default in standards layer).
- Deterministic seeding of mock time drifts (ppm) to exercise convergence and holdover logic in DES-C-002.

## Traceability

Implements timing primitives required by:

- DES-C-001 DARS Protocol Core (TRP generation and jitter metrics)
- DES-C-002 Synchronization Manager (master selection, degradation, and holdover)
- DES-D-001 DARS Info Model (immutable snapshots)
- DES-D-002 Timing Reference Model (windowed interval calculations)

Related requirements: REQ-F-DARS-001/002/003, REQ-F-SYNC-001/002/003/004, REQ-NF-PERF-001, REQ-NF-REL-001, REQ-NF-PORT-001.

## Rationale and references

- AES-11-2009 sections on timing reference point and identification are respected by ensuring atomic timing snapshots and AES5-aligned rates.
- AES5-2018 preferred sampling frequencies guide the allowed sample_rate_hz values.

Copyright‑friendly note: This design references standards by section numbers and implements behavior based on understanding; it does not reproduce any copyrighted text or tables.
