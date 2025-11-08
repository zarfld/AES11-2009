---
title: "Critical Items List (CIL) - AES11 DARS Protocol"
specType: cil
standard: IEEE 1633-2016
phase: 05-implementation
status: draft
version: 0.1.0
component: AES::AES11::_2009::dars
author: GitHub Copilot AI
created: 2025-11-08
updated: 2025-11-08
traceability:
  sfmea: docs/reliability/sfmea-AES11-DARS-2009.md
  operationalProfile: docs/reliability/operational-profile-AES11-DARS-2009.md
---

## 1. Scope

Critical Items List capturing all SFMEA entries with RPN ≥ 200 that must be mitigated prior to release.

## 2. Items

| CIL ID | FMEA ID | Description | RPN | Owner | Mitigation Plan | Due | Status |
|--------|---------|-------------|-----|-------|------------------|-----|--------|
| CIL-001 | FM-002 | Timing reference drift beyond Grade 1 tolerance | 270 | Core Protocol | Add continuous drift monitor + recalibration | Phase 05 | Open |
| CIL-002 | FM-004 | Holdover drift exceeds Grade 2 tolerance | 224 | Core Protocol | Adaptive holdover estimator & alarm | Phase 06 | Planned |
| CIL-003 | FM-007 | Counter overflow in reliability metrics | 200 | Common Metrics | Upgrade to 64-bit, overflow detection log | Phase 05 | Open |

## 3. Verification & Closure Criteria

- Mitigation implemented and covered by unit/integration tests
- Related OP coverage targets still met or improved
- No regression in CI gates (coverage ≥80%, static analysis clean)
- Evidence captured in test artifacts under `07-verification-validation/`

## 4. Notes

- CIL is living; items can be split/merged as design evolves
- RPN recalculation required if severity/occurrence/detectability change based on data

