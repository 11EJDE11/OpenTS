---
key: CD
scope: campaign
label: Campaign disc
see_also: ["Scenario", "FinalMovie", "RequiredAddon"]
when_omitted:
  kind: value
  value: "-1"
---

`0` names the GDI disc, `1` the Nod disc and `2` the Firestorm disc, while `-1` names no disc in particular. Starting any mission of the campaign makes that disc the one the game insists on before the map is read, and unless the build treats every data file as local the player is asked for it when it is not in a drive.

Two further decisions read the same number. The opening cinematic plays ahead of the first mission's briefing only while the value is below `2`. The loading screen picks its backdrop from a pair of GDI pictures for `0` and a pair of Nod pictures for `1`; anything above `1` falls back to searching the campaign's opening scenario file name for `GDI`, taking the GDI pair when it is found and the Nod pair when it is not.

:::caution[A campaign that names no disc reads its backdrop name from outside the table]
The backdrop table is indexed straight from the disc number, so `-1` lands two places in front of it. The offsets used at 640 by 480, and at 800 by 600 or larger, carry the index back inside the table, but at every other screen size the file name is taken from the bytes in front of it.
:::
