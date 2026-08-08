---
key: StretchMovies
summary: Stretches full screen movies to fill the display instead of playing them at their own size.
see_also: [ScreenWidth, ScreenHeight]
when_omitted:
  kind: value
  value: "no"
---

The flag reaches the full screen movie player alone, and only where that player's caller also asks for stretching. A movie played into a fixed rectangle — the sidebar's, for one — never consults the flag and keeps that rectangle either way.

The read turns the flag off again on a display that cannot stretch while blitting, so on such a display the stored setting never takes effect and the display options screen shows the check box disabled and clear.

The display options screen carries the same switch and stores it as the screen is accepted; leaving the options screen behind it writes the setting back to `sun.ini`.
