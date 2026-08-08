---
key: Side
scope: housetype
label: Country side
see_also: [Multiplay, Crew, Technician]
when_omitted:
  kind: inherited
  note: The side whose [Sides] entry lists this country, or no side at all when none does.
---

```ini title="rules.ini"
[Nod]
Side=Nod
```

The `[Sides]` section is read first and points every country it lists at its side; this key is read afterwards and overrides that pairing for the one country. A country left with no side at all is not an error — that is what the stock civilian and mutant countries would carry if `[Sides]` did not list them.

The side is a coarser grouping than the country, and only three things ask for it. A musical score restricted with [`Side=`](/keys/side/#scope-themes) is offered only while the local player's country belongs to that side. The score screen draws its artwork from it. And whether an object's survivor is a [`Crew`](/keys/crew/) or a [`Technician`](/keys/technician/) turns on whether its owner's country has a side at all.

:::caution[An unrecognized name creates a side rather than being rejected]
A value naming no declared side adds a side under that name and puts the country in it. Nothing else joins it, and the country stops matching the sides that the score screen and the music playlist test against — so a typo silently moves a country out of both.
:::

:::caution[Changing a country's side leaves the old side's roster wrong]
When this key moves a country off the side `[Sides]` gave it, the country is added to the new side's roster and an entry is removed from the old one by *position* — the position matching the country's own place in the country list, which is rarely the entry that should go. A later rules file that names a side inside a `[Sides]` entry then expands the wrong set of countries.
:::
