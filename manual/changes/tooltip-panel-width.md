---
title: Draw a tooltip that fills the width of its panel
category: fix
release: 0.1.0
targets:
- type: system
  id: sidebar
  effect: changed
credit:
- ZivDero
---

A tooltip whose text comes to the full width of the sidebar is now drawn. A tooltip is kept
within whichever of the sidebar and the tactical view it sits over, so one wide enough to
reach the far edge is moved back until it ends exactly there. The test that then picks which
of the two to draw it on required the tooltip to begin past the boundary between them rather
than at it, so a tooltip left flush against that boundary was taken to cross it and was drawn
on neither.

A build cameo whose name and price together come to the width of the sidebar showed nothing
at all, however long the pointer rested on it.
