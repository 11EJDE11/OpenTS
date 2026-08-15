---
title: Replace the fastmath lookup tables
category: internal
release: 0.1.0
targets: []
---

OpenTS now uses the C runtime trigonometric and square-root functions in place
of the inherited fastmath lookup tables. Save and packet formats are unchanged,
but numerical results may differ.

Peers must use matching OpenTS versions. Replays or mid-mission saves recorded
with the table implementation may diverge when continued with the runtime
implementation.
