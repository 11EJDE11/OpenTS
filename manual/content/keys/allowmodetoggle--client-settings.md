---
key: AllowModeToggle
scope: client-settings
label: Mode switching setting
see_also: [ScreenWidth, ScreenHeight]
when_omitted:
  kind: unchanged
  note: The read passes through the setting the startup read established, and nothing later overwrites it.
---

This is the later of the two reads of the assignment, made with the rest of the client settings once the display is already open. It cannot change how the game opened; it settles whether a scenario switches the display when it starts and when it ends, and whether the display options screen tests a chosen mode before storing it.

The two reads differ in what they fall back on. This one keeps the flag as the startup read left it, so an assignment present in the file is read twice to the same answer and one absent from it changes nothing here.
