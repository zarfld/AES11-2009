---
title: "Health Checks Scope - AES11 DARS Standards Layer"
phase: 05-implementation
status: draft
version: 0.1.0
author: GitHub Copilot AI
component: AES::AES11::_2009::dars
created: 2025-11-08
traceability:
  design: [DES-C-001, DES-C-002]
  requirements: [REQ-NF-RELIABILITY-001]
  operationalProfile: docs/reliability/operational-profile-AES11-DARS-2009.md
---

## 1. Purpose

Clarify health check responsibility boundaries: The AES-11 DARS standards layer is protocol logic only (generation, validation, timing reference). _It does not expose network, process, or hardware health endpoints._ Health checks are provided by the integration/service layer that embeds this library.

## 2. Responsibilities Split

| Layer | Responsibilities | Health Signals Origin | Notes |
|-------|------------------|------------------------|-------|
| Standards (this repo) | Pure AES-11 protocol logic; timing reference generation/validation; metrics counters | Provides raw metrics counters & state flags | No direct HTTP / RPC endpoints |
| Integration / Service | Aggregates protocol metrics, implements HTTP/gRPC health endpoints, collects drift and holdover status, exposes readiness/liveness | Constructs composite status from library metrics | May add circuit breaker state & external dep status |
| Operations / Deployment | Monitors endpoint responses, handles alerting/escalation | Consumes service layer health API | Out-of-scope here |

## 3. Provided Signals (Library)

Minimal signals (to be extended):

- `dars_frame_generation_failures` counter
- `dars_frame_validation_failures` counter
- `holdover_entry_count` (planned)
- `timing_drift_ppm` (planned continuous monitor)
- `degraded_state_flag` (planned)

## 4. Suggested Service Layer Health Endpoints

| Endpoint | Type | Composition | Threshold Example |
|----------|------|------------|-------------------|
| `/health/live` | Liveness | Always `OK` if process responding | N/A |
| `/health/ready` | Readiness | `OK` if not in prolonged holdover (> N seconds) and drift ppm <= grade tolerance | drift_ppm <= 10 (Grade 2) |
| `/health/sync` | Detailed | JSON (grade, drift_ppm, holdover, degraded) | holdover_duration < configured limit |
| `/metrics` | Metrics | Prometheus exposition (maps counters) | N/A |

## 5. Data Flow

```text
+--------------------+        +----------------------+        +------------------+
| Standards Layer    | --->   | Service Layer        | --->   | Ops Monitoring    |
| (metrics + states) |        | (health aggregation) |        | (dashboards/alert)|
+--------------------+        +----------------------+        +------------------+
```

## 6. Health Evaluation Logic (Service Layer Pseudocode)

```cpp
HealthStatus evaluateReadiness(const ProtocolMetrics& m, const TimingState& t) {
  if (t.inHoldover && t.holdoverDurationSec > cfg.maxHoldoverReadySeconds) {
    return HealthStatus::UNREADY("Holdover exceeds limit");
  }
  if (t.driftPpm > cfg.grade2MaxPpm) {
    return HealthStatus::DEGRADED("Drift beyond Grade 2 tolerance");
  }
  if (m.frameGenerationFailuresRateLastMin > cfg.maxGenFailRate) {
    return HealthStatus::DEGRADED("Frame generation failure rate high");
  }
  return HealthStatus::READY();
}
```

## 7. Out-of-Scope Items

- Hardware clock discipline primitives (PTP, GPS) — separate repositories
- Network transport availability checks
- External storage/database readiness
- Authentication/authorization subsystem health

## 8. Future Extensions

| Extension | Description | Phase Target |
|-----------|-------------|--------------|
| State Entry Counters | Per-state counts to refine readiness heuristics | Phase 06 |
| Transition Probability Export | Feed probabilistic degradation early warning | Phase 06 |
| Drift Trend Classification | Laplace/shape check integration for drift anomalies | Phase 07 |

## 9. Compliance Notes

No AES or IEEE copyrighted text reproduced. Health checks derived from general reliability engineering practice & internal requirements understanding.

