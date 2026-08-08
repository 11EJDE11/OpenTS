---
key: CaptureTheFlag
summary: Whether a modem game starts with capture-the-flag turned on.
see_also: [BaseUnit, Bases]
when_omitted:
  kind: value
  value: "no"
---

One setup screen reads the value into the flag the game logic actually consults: the host's screen for a modem game, which then sends its whole set of scenario flags to the guest. The screen writes it every time it opens, and nothing on the screen can change it afterwards. No other setup screen writes that flag, and nothing resets it between matches, so a skirmish, network or online match runs with capture-the-flag off unless a modem host screen was opened earlier in the same run of the game.

With the flag on, and with [`Bases`](/keys/bases/) on as well, each house's [`BaseUnit`](/keys/baseunit/) is given that house's own flag as it is placed. Carrying it halves the unit's speed and stops it cloaking — it shimmers instead — and the flag drops back onto the ground when the unit deploys or is otherwise taken off the map. A house that is defeated has its flag taken away.

The screen's write reaches the scenario's own copy only at the end of the scenario read, after the base units have been placed, so the flags appear from the following match rather than the one the screen set up.

:::caution[Nothing captures a flag]
No routine moves a flag from one house to another, and no victory condition reads one. Turning the mode on marks the starting base units and slows them down; the contest the name describes is not in the engine.
:::

The `[MultiplayerDefaults]` value also seeds a session option of the same name that is exchanged between machines during setup, but no gameplay path reads that option.
