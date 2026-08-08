---
key: CD
scope: multiplayer-maps
label: Loose map discs
see_also: ["Description", "MinPlayers", "Official"]
when_omitted:
  kind: computed
  note: The hard drive. A map that lists no disc is given the local-drive entry once the read has finished.
---

A loose `.MPR` in the game directory is listed in its own right, and the disc list is read from its `[Multiplay]` section rather than from any packet. The value is a comma-separated list of disc numbers, each read as a plain integer: `0` for the GDI disc, `1` for the Nod disc, `2` for the Firestorm disc, `-1` for any disc and `-2` for the hard drive.

```ini title="MyMap.MPR"
[Multiplay]
Description=Four player canyon
CD=-2
```

The list decides which disc the game asks for when the map file cannot be found. Asked about the hard drive, the answer ignores the list and simply reports whether the file is present; asked about any disc, a list holding `-1`, `0` or `1` answers yes whichever disc was asked about.

The whole section is read only while the map file is available, so a listing built for a file that has since gone missing keeps the hard-drive entry it was given by default.
