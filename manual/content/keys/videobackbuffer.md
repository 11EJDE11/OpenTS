---
key: VideoBackBuffer
summary: Parsed video flag that the engine never uses.
no_effect: true
see_also: [ScreenWidth, StretchMovies]
when_omitted:
  kind: value
  value: "yes"
---

The name promises a say in whether the display is given a back buffer to draw into. The flag is read before the display is prepared, which is the one moment such a choice could be acted upon, and is stored in a global that starts out set. Saving the options does not write it back out, and nothing reads that global afterward.
