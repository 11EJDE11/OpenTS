---
key: LaserInnerColor
summary: The color of the thin core line of a laser beam.
see_also: ["IsLaser", "LaserOuterColor", "LaserOuterSpread", "LaserDuration"]
when_omitted:
  kind: value
  value: "0,0,0"
---

Three channel values from 0 to 255 give the core of the beam its color. The core is drawn once, as a single antialiased line straight down the middle of the beam, and it is the only part of a laser whose color is chosen freely — the flanking glow blends its red alone, and so does the screen glow that spans the same line.

```ini title="rules.ini"
[MyObeliskRay] ; example WeaponType
IsLaser=yes
LaserInnerColor=255,0,0
```

Only the channels above zero are blended onto the terrain, so a channel left at zero contributes nothing and a color of `0,0,0` leaves no core at all. The lowest detail setting drops that test along with the blending: the core becomes a plain shaded line in exactly this color, and a black core is drawn as a black line there.

The figure is read from the weapon in the object's first slot whichever slot fired, so a laser weapon in the second slot borrows the first weapon's core color.

:::caution[A partial triplet leaves the remaining channels unpredictable]
`LaserInnerColor=255` gives a core whose green and blue come from whatever was last in that storage; [INI syntax](/formats/ini-syntax/#malformed-values) explains the incomplete-value defect behind it, and why leaving the key out is safe.
:::
