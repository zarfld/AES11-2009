---
title: "Operational Profile - AES11 DARS Protocol"
specType: operational-profile
standard: IEEE 1633-2016
phase: 05-implementation
status: draft
version: 0.1.0
author: GitHub Copilot AI
component: AES::AES11::_2009::dars
scope: "Software-only, hardware-agnostic AES-11-2009 DARS generation/validation layer"
created: 2025-11-08
updated: 2025-11-08
traceability:
  design: [DES-C-001, DES-C-002, DES-C-003]
  requirements: [REQ-F-DARS-GEN-001, REQ-F-DARS-VAL-002, REQ-NF-RELIABILITY-001]
  tests: [TEST-DARS-STATE-001, TEST-SYNC-SELECT-001, TEST-SYNC-RESELECT-001]
references:
  - 01-stakeholder-requirements/requirements-reliability.md
  - 02-requirements/system-requirements-specification.md
  - 03-architecture/architecture-quality-scenarios.md
  - 07-verification-validation/TEST-DARS-STATE-001.md
copyrightNotice: |
  Implements operational profile based on understanding of AES-11-2009 and IEEE 1633 reliability guidance. No copyrighted specification text reproduced.
---

## 1. Actors and User Segments

| Segment | Role / Description | Usage Share (%) | Reliability Criticality |
|---------|--------------------|-----------------|-------------------------|
| StudioMaster | Central facility clock controller using DARS for global sync | 25 | High (sync collapse risk) |
| BroadcastChain | Automated playout / ingest pipeline referencing DARS | 30 | High (continuous operation) |
| MobileRig | Remote capture system locking to facility DARS | 15 | Medium (mission time bounded) |
| PostProduction | Offline editors validating clip alignment via DARS timestamps | 20 | Low (non real-time) |
| LabQA | Conformance / characterization bench exercising edge cases | 10 | Medium (stress & fault injection) |

## 2. Operations and Modes

Top-level protocol operations (software component scope only):

| Operation ID | Operation / Mode | Description | Relative Frequency (%) | Actor Coverage |
|--------------|------------------|-------------|------------------------|----------------|
| OP-001 | InitializeProtocol | Establish internal state tables & sampling rate context | 5.0 | All |
| OP-002 | GenerateDARSFrame | Construct DARS frame with timing reference point | 40.0 | StudioMaster, BroadcastChain |
| OP-003 | ValidateIncomingFrame | Parse AES3 frame, verify DARS identification & timing | 28.0 | BroadcastChain, MobileRig |
| OP-004 | UpdateTimingReference | Recompute reference after drift / reselect event | 8.0 | StudioMaster |
| OP-005 | HoldoverModeEntry | Enter degraded mode on loss of upstream reference | 3.0 | BroadcastChain, MobileRig |
| OP-006 | HoldoverModeMaintenance | Maintain timing within tolerance during holdover | 6.0 | BroadcastChain, MobileRig |
| OP-007 | ReselectSource | Switch to alternate time source after recovery | 4.0 | StudioMaster, BroadcastChain |
| OP-008 | FaultInjectionToggle | Enable/disable named fault points (test only) | 2.0 | LabQA |
| OP-009 | MetricsSnapshot | Capture reliability counters for logging | 4.0 | LabQA, StudioMaster |
| OP-010 | DegradationAssessment | Evaluate jitter / ppm vs grade thresholds | 0.5 | LabQA |
| OP-011 | ShutdownProtocol | Graceful flush & final metrics emission | 1.5 | All |

80/20 Observation: OP-002 + OP-003 ≈ 68% of usage volume; optimization & test focus prioritized there.

## 3. Mission Profiles

Representative mission contexts:

| Mission ID | Description | Duration | Dominant Ops | Notes |
|------------|-------------|----------|--------------|-------|
| M-001 | Broadcast 24h cycle | 24h continuous | GenerateDARSFrame, ValidateIncomingFrame | High stability emphasis |
| M-002 | Mobile event capture | 6h window | ValidateIncomingFrame, HoldoverModeMaintenance | Intermittent upstream loss possible |
| M-003 | Studio clock maintenance | 2h maintenance | UpdateTimingReference, ReselectSource | Manual operator interventions |
| M-004 | Conformance bench | 4h scripted | FaultInjectionToggle, MetricsSnapshot, DegradationAssessment | Stress sequences & edge triggering |
| M-005 | Post-production alignment | 8h editing | ValidateIncomingFrame | Non real-time, tolerant to retry |

## 4. Behavioral Model (States / Transitions)

States (Markov Chain Usage Model abstraction):

| State ID | Name | Description |
|----------|------|-------------|
| S-000 | Uninitialized | Library not yet set up |
| S-001 | Initialized | Base configuration loaded |
| S-002 | Generating | Actively producing DARS frames |
| S-003 | Validating | Parsing & verifying incoming frames |
| S-004 | Holdover | Operating without fresh external reference |
| S-005 | Reselecting | Transitioning to alternate time source |
| S-006 | Degraded | Timing accuracy outside nominal, within fail-safe |
| S-007 | Shutdown | Finalization phase |

Transitions (subset with qualitative frequency buckets):

| Transition ID | From | Event | To | Frequency Bucket | Notes |
|---------------|------|-------|----|------------------|-------|
| T-001 | Uninitialized | InitializeProtocol | Initialized | very-often | Startup |
| T-002 | Initialized | StartGeneration | Generating | often | Normal activation |
| T-003 | Initialized | StartValidation | Validating | often | Passive validation mode |
| T-004 | Generating | LossOfReference | Holdover | seldom | Upstream failure |
| T-005 | Holdover | ReferenceRestored | Reselecting | normal | Recovery begins |
| T-006 | Reselecting | ReselectComplete | Generating | normal | Resume generation |
| T-007 | Validating | ReferenceDriftDetected | Degraded | normal | Tolerance breach |
| T-008 | Degraded | DriftCorrectionApplied | Validating | normal | Auto correction successful |
| T-009 | Any Active | ShutdownProtocol | Shutdown | normal | End of session |
| T-010 | Holdover | ExtendedLoss | Degraded | rare | Accuracy degrading over time |
| T-011 | Generating | FaultInjected | Degraded | rare | LabQA induced fault |

Normalization: Exact probabilities to be computed once empirical telemetry collected (Phase 06). Frequency buckets guide test generation priority.

## 5. Transition Probability Normalization Plan

Procedure:
 
1. Instrument counters per transition event.
2. Collect telemetry over representative mission runs (M-001..M-004).
3. Aggregate counts; per-state outgoing transitions normalized to 1.0.
4. Store probability table in `reports/operational-profile-probabilities.json`.
5. Use table to parameterize usage-based test generation in Phase 06.

## 6. Coverage Targets (Operational Profile Driven)

| Coverage Dimension | Target |
|--------------------|--------|
| State Coverage | ≥ 100% (all states exercised) |
| Transition Coverage | ≥ 95% (rare paths acceptable residual) |
| High-Frequency Ops (OP-002, OP-003) | ≥ 99% reliability test execution proportion |
| Degraded/Holdover Paths | ≥ 90% triggered in stress suites |
| Fault Injection Variants | All defined fault points at least once per nightly run |

## 7. Test Generation Mapping

Example abstract action adapters (future automation):

```yaml
abstractAction: GenerateDARSFrame
adapter: tests/adapters/dars/generate_frame.cpp
verifications:
  - assert frame.dars_identified == true
  - assert timing_ref_point_accuracy_ppm <= 1  # Grade 1 target
  - assert metrics.frame_generation_failures == 0
```

```yaml
abstractAction: EnterHoldover
adapter: tests/adapters/dars/enter_holdover.cpp
verifications:
  - assert state == Holdover
  - assert drift_ppm <= 10  # Grade 2 tolerance
  - assert holdover_entry_count incremented
```

Mapping table (initial placeholder):

| Operation ID | Abstract Action | Adapter Planned | Status |
|--------------|-----------------|-----------------|--------|
| OP-002 | GenerateDARSFrame | generate_frame.cpp | planned |
| OP-003 | ValidateIncomingFrame | validate_frame.cpp | planned |
| OP-005 | EnterHoldover | enter_holdover.cpp | planned |
| OP-007 | ReselectSource | reselect_source.cpp | planned |
| OP-008 | InjectFault | inject_fault.cpp | existing (test_fault_injection.cpp) |
| OP-009 | SnapshotMetrics | snapshot_metrics.cpp | planned |

## 8. Reliability Data Collection

| Aspect | Method | Notes |
|--------|--------|-------|
| Operation Counts | Atomic counters (metrics module) | Already partially implemented |
| Failure Counts | Metrics increments on failure paths | Extend for each new failure mode |
| Transition Events | Add lightweight event sink emission | Future expansion (Phase 06) |
| Timing Drift | Capture ppm deviations per frame batch | Derive from reference timestamps |
| Holdover Duration | Timestamp on entry/exit; compute delta | Degradation aging modeling |
| Fault Injection Impact | Compare reliability counters pre/post fault | Supports accelerated test models |

MTBF Estimation Approach: Use AMSAA (Duane) model initial fit once ≥ 20 failures recorded in test environment; refine with SRG in Phase 07.

## 9. Variants and Evolution Strategy

Versioning:

- OP document version increments on structural changes (states or operations added/removed).
- Probability table regenerated post-change with at least 3 mission run samples.

Impact Analysis:
 
1. Identify affected operations/states.
2. Update adapter mapping and test selection weights.
3. Recalculate coverage deltas; ensure targets sustained.

 
## 10. References

- IEEE 1633-2016 (Software Reliability Engineering) – conceptual alignment only.
- AES-11-2009 (Digital Audio Reference Signals) – protocol context for operations.
- Internal: SRPP (software-reliability-program-plan.md), architecture quality scenarios, traceability matrix.

## 11. Open Items / Next Steps

| Item | Description | Phase Target |
|------|-------------|--------------|
| OP-NORM-001 | Implement transition counters & probability export | Phase 06 |
| OP-ADAPT-002 | Build adapter layer for abstract actions | Phase 06 |
| OP-DRIFT-003 | Add drift ppm calculation & metric emission | Phase 05 (pending) |
| OP-HOLD-004 | Implement holdover aging model | Phase 06 |
| OP-PROB-005 | Initial normalization after telemetry collection | Phase 06 |
| OP-METRICS-006 | Extend metrics for state entry counts | Phase 05 (pending) |

## 12. Compliance Notes

All content is original; no reproduction of AES or IEEE specification text. Operational profile constructed from internal requirement understanding and reliability engineering best practice.

