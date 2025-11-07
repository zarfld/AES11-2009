---
specType: guidance
title: C4 Level 3: Component Diagram (Standards Library)
author: "AI Assistant"
date: "2025-11-07"
---

## C4 Level 3: Component Diagram (Standards Library)

```mermaid
graph TB
    Core[ARC-C-001 Standards Core] --> SyncMgr[ARC-C-004 Synchronization Manager]
    Core --> DGen[ARC-C-002 DARS Generator]
    Core --> DVal[ARC-C-003 DARS Validator]
    Core --> Vid[ARC-C-005 Video Sync Alignment]
    Core --> Conf[ARC-C-006 Conformance Harness]
    Core --> AES3[AES3 Adapter]
    Core --> AES5[AES5 Adapter]

    HAL[HAL Facades (interfaces)] --> Core
    AES3 -. external .- A3[AES3-2009 Repo]
    AES5 -. external .- A5[AES5-2018 Repo]
```

## Key Responsibilities

- Synchronization Manager: state machine (acquire/lock/holdover)
- DARS Generator/Validator: frame creation/validation, TRP handling
- Video Sync Alignment: alignment to video sync pulse
- Conformance Harness: orchestrates section 5/6 tests
