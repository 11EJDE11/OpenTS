---
title: Honor a map's MaxPlayers setting
category: fix
release: 0.1.0
targets:
- type: key
  id: MaxPlayers
  effect: added
---

A multiplayer map's `MaxPlayers` field now fills the largest number of players the map
declares. Both places that read it asked for `MinPlayers` instead, so a listing's ceiling
always came out equal to its floor, and a map could not declare that it accepted more
players than it required.

Neither limit is enforced. Nothing consults either count once the listing has been built,
so what this settles is what a map declares rather than how many players may join it.
