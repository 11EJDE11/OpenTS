---
key: Theater
scope: scenarios
label: Scenario theater
see_also: [IceGrowthEnabled]
when_omitted:
  kind: value
  value: "TEMPERATE"
---

```ini title="map file"
[Map]
Theater=SNOW
```

The theater is settled before any of the map's contents are read, because it decides which tile, art and palette archives are mounted for the whole load. Two are registered, `TEMPERATE` and `SNOW`, and the match ignores case. Beyond the artwork it also gates the snow-only behavior in the simulation, so [`IceGrowthEnabled`](/keys/icegrowthenabled/) does nothing on a temperate map.

Unlike the rest of the scenario's settings, this one lives in the map's `[Map]` section.

:::danger[An unrecognized theater name is used as an index anyway]
A name matching neither registered theater resolves to "no theater", which is stored as `-1`. That value is then used unchecked to look up the archive names, reading in front of the table instead of inside it and handing whatever it finds to the archive loader.
:::
