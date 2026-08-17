---
title: Keep the elapsed mission time across a save game
category: fix
release: 0.1.0
targets: []
credit: [ZivDero]
---

The elapsed mission time survives a save and load. It previously reset to zero whenever a
game was loaded, so the time reported at the end of a mission counted only from the last
load rather than from the start of the mission.

The saved value was read back correctly and then discarded: the scenario was reassembled
after the read, and the elapsed timer was one of the few members that step did not
preserve.
