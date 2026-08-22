---
title: Make seeing through a disguise a stated ability
category: feature
release: 0.1.0
targets:
- type: key
  id: DetectDisguise
  effect: added
- type: key
  id: AIDetectDisguise
  effect: added
- type: key
  id: Disguised
  effect: changed
- type: system
  id: target-selection
  effect: changed
credit: [ZivDero]
---

Two settings now decide who sees through a disguise. A type set `DetectDisguise=yes` scores a
disguised soldier like any other candidate, and `AIDetectDisguise=yes` in `[AI]` gives every
computer-controlled house the same sight while leaving a player's own units passing the
soldier over. Both are off unless the rules ask for them, so nothing changes for a game that
states neither.

The rejection used to read the candidate's type alone, with no way to grant an exception —
not even to the dog a source comment claimed was already exempt. Ability now belongs to
whatever the rules name, rather than to nothing at all.
