---
key: AllowModeToggle
scope: client-settings-2
label: Mode switching at startup
see_also: [ScreenWidth, ScreenHeight]
when_omitted:
  kind: value
  value: "no"
---

This is the earlier of the two reads of the assignment, and the one that decides how the game opens. Turned on, the main window and the video mode are created at 640 by 400 — 640 by 480 if the display refuses that — rather than at the configured screen size, which is switched to only as a scenario starts and switched back as it ends. Turned off, the display opens at the configured size and stays there for the whole run.

The flag also relaxes the display options screen: with switching allowed, a mode picked there is stored without first being tested on a trial display, so a mode the adapter cannot really produce is accepted quietly.

The read takes `no` as its answer when the assignment is absent, whatever the flag already held. Saving the options does not write the flag back out, so a setting written by hand survives; typing `TOGGLE` at the main menu flips it for that run, and unlike the other codes it survives into a network game.
