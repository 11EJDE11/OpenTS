---
title: Add a Place Building hotkey command
category: feature
release: 0.1.0
targets:
- type: command
  id: ManualPlace
  effect: added
credit: [CCHyper, ZivDero]
---

A building that has finished construction and sits waiting on the sidebar can now be picked
up from the keyboard. Placing one previously meant clicking its completed cameo, so a player
who had dismissed the placement cursor had to travel back to the sidebar to recover it.

The command arrives unbound. It appears under Interface in the keyboard options, where it
takes whatever key the player assigns; no existing keyboard file is disturbed, since none
names it. It does nothing while no building is complete, while the finished item is not a
building, or while that building is already being placed.

A superweapon cursor armed at the time is dropped, so placement begins rather than the
superweapon reasserting its cursor once the building is down.
