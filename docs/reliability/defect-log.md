---
title: "Defect Log & DDR Tracking - AES11 DARS"
specType: reliability-defects
phase: 05-implementation
status: draft
version: 0.1.0
component: AES::AES11::_2009::dars
author: GitHub Copilot AI
updated: 2025-11-08
traceability:
  srpp: spec-kit-templates/software-reliability-program-plan.md
  sfmea: docs/reliability/sfmea-AES11-DARS-2009.md
  cil: docs/reliability/cil-AES11-DARS-2009.md
---

## Purpose

Track discovered defects with severity to support Software Reliability Engineering (IEEE 1633). Compute Defect Discovery Rate (DDR = defects/KLOC) from a machine-readable log and code size snapshot.

## Files

- Machine-readable log: `docs/reliability/defect-log.yaml`
- Metrics artifact (generated): `build/defect-metrics.json`

## Severity Scale (FDSC-aligned)

- Critical (10): Safety hazard / mission failure / data loss
- High (7–9): Major function degraded
- Medium (4–6): Minor function impacted; workaround
- Low (1–3): Cosmetic / no functional impact

## Process

1. Append defects to `defect-log.yaml` with fields: id, title, component, severity, discoveredOn, status.
2. CI step runs `scripts/ci/compute-ddr.py` which:
   - Counts code size (KLOC) for `lib/` sources
   - Computes DDR = totalDefects / KLOC
   - Emits `build/defect-metrics.json` and prints a summary
   - Optionally enforces a max DDR threshold if `DDR_MAX` env is set (float)

## Initial Threshold

- Threshold is governed by SRPP Section 4.2. If unset, computation is informative only.

