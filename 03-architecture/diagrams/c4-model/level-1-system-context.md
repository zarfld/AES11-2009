---
specType: guidance
title: C4 Level 1: System Context
author: "AI Assistant"
date: "2025-11-07"
---

## C4 Level 1: System Context

```mermaid
graph TB
    U[User / Integrator]:::actor
    T[Conformance Test Equipment]:::ext
    V[Video Sync Source]:::ext
    G[GPS 1PPS Source]:::ext
    AES11[AES-11 Standards Library\nAES::AES11::_2009]:::system
    AES3[AES3-2009 Library]:::ext
    AES5[AES5-2018 Library]:::ext

    U -->|configure, run tests| AES11
    AES11 -->|uses frames| AES3
    AES11 -->|uses rates| AES5
    V -->|sync reference| AES11
    G -->|timing reference| AES11
    T -->|measure, validate| AES11

    classDef system fill:#247ba0,stroke:#0b3c49,stroke-width:1,color:#fff
    classDef actor fill:#7cb518,stroke:#2b9348,color:#000
    classDef ext fill:#d0d0d0,stroke:#666,color:#000
```
