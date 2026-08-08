---
key: AllowHiResModes
summary: Offers every display mode the adapter reports rather than the two the game normally allows.
see_also: [ScreenWidth, ScreenHeight, StretchMovies]
when_omitted:
  kind: value
  value: "no"
---

The flag governs the list the display options screen builds. Turned off, the list is enumerated up to 800 by 600 and then narrowed again to the modes exactly 640 or exactly 800 pixels wide, which is the pair of choices the game normally offers. Turned on, the list is enumerated up to 4096 by 4096 and every 16-bit mode the adapter reports is offered.

The flag has no other reader: it decides what may be chosen, not how anything is drawn. A resolution outside the usual two can equally be written straight into `sun.ini` as [`ScreenWidth`](/keys/screenwidth/) and [`ScreenHeight`](/keys/screenheight/), and the flag is not consulted when those are read.

The options screen has no control for the flag, and saving the options does not write it back out, so a setting written by hand survives. Typing `HIRES` at the main menu flips it for that run, and starting a network game turns it off again along with the rest of the single player indulgences.
