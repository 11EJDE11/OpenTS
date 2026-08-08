---
key: ScreenWidth
scope: client-settings-2
label: Width the display opens at
see_also: [ScreenHeight, AllowModeToggle, AllowHiResModes]
when_omitted:
  kind: computed
  note: Both dimensions become 640 by 480 when either is left out, and 640 by 400 instead on a Windows release outside the NT family that reports no more than two megabytes of video memory.
---

This is the earlier of the two reads of the assignment, made before the main window exists. Either dimension left out or written as `-1` replaces both, so a width alone is never enough to size the screen. The main window and the full screen video mode are then created at the resulting size. A mode the display cannot set falls back to 640 by 480; if that fails too, the game reports a video error and stops.

The size also fixes the layout the game is drawn into: the sidebar takes a fixed 168 pixels of the width, and the tactical view is given what is left.

With [`AllowModeToggle`](/keys/allowmodetoggle/) turned on, the display opens at 640 by 400 whatever this assignment says, and the configured size is switched to only as a scenario starts.
