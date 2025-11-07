---
specType: guidance
title: C4 Level 2: Container Diagram
author: "AI Assistant"
date: "2025-11-07"
---

## C4 Level 2: Container Diagram

```mermaid
graph TB
    subgraph Standards_Layer
      C1[Core Library\n(core/dars/sync/video)]
      C2[Conformance Harness\n(conformity)]
    end

    subgraph Service_Layer (External)
      H1[HAL Adapter(s)\n(ASIO/ALSA/CoreAudio/RTOS)]
    end

    subgraph External_Standards
      A3[AES3-2009 Library]
      A5[AES5-2018 Library]
    end

    U[Integrator/Tests] --> C2
    C2 --> C1
    C1 --> A3
    C1 --> A5
    H1 --> C1
```
