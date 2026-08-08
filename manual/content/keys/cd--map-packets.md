---
key: CD
scope: map-packets
label: Packet-listed map discs
see_also: ["Description", "MinPlayers"]
when_omitted:
  kind: computed
  note: No disc at all. The packet entry starts with an empty disc list and nothing fills it in, unlike a loose map file, which falls back to the hard drive.
---

The value is a comma-separated list of disc numbers, each read as a plain integer: `0` for the GDI disc, `1` for the Nod disc, `2` for the Firestorm disc, `-1` for any disc and `-2` for the hard drive. Text that is not a number reads as `0`.

```ini title="MyMaps.PKT"
[MultiMaps]
1=MYMAP

[MYMAP] ; the section the entry above named
Description=Four player canyon
CD=0,1
```

The list decides which disc the game asks for when the map file itself cannot be found. Asked about the hard drive, the answer ignores the list entirely and simply reports whether the file is present; asked about any disc, a list holding `-1`, `0` or `1` answers yes whichever disc was asked about, so only a list confined to `2` restricts a map to the Firestorm disc.

:::danger[A packet-listed map with no `CD` reads the first entry of an empty list]
The constructor that builds an entry from a packet clears the disc list and adds nothing when the assignment is missing. The routine that reports which disc to ask for returns the first element regardless, and it is reached exactly when the map was not found on the current disc — so a map listed in a packet, absent from the drive, and carrying no `CD` reads through an empty list.
:::
