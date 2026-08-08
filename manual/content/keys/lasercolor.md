---
key: LaserColor
summary: The color of the beam a railgun system draws.
see_also: [BehavesLike, Laser, LaserInnerColor, LaserOuterColor]
when_omitted:
  kind: value
  value: "0,0,0"
---

Three comma-separated channel values from `0` to `255`. The beam is drawn as a single line in this color, with none of the glow a weapon's own beam carries, so this is the whole of its appearance. It is read only where [`Laser=yes`](/keys/laser/) is set on a `Railgun` [system](/keys/behaveslike/#scope-particlesystemtype); every other behavior ignores it.

```ini title="rules.ini"
[MyRailgunSys] ; a ParticleSystemType registered in [ParticleSystems]
BehavesLike=Railgun
HoldsWhat=MyRailgunPart ; a ParticleType registered in [Particles]
Laser=yes
LaserColor=255,128,0 ; orange
```

Above the lowest detail setting a channel set to zero is not blended at all: the pixels the beam crosses keep their own value on that channel. `255,128,0` therefore tints what it crosses red and green and leaves the blue beneath it alone, and a color left at black draws no visible beam whatever [`Laser=yes`](/keys/laser/) asks for. The lowest setting drops the rule and paints the line flat in the color as given, so black draws a black beam there.

:::caution[A partial triplet leaves the remaining channels unpredictable]
`LaserColor=255` gives a beam whose green and blue come from whatever was last in that storage; [INI syntax](/formats/ini-syntax/#malformed-values) explains the incomplete-value defect behind it, and why leaving the key out is safe. A channel above `255` wraps around rather than being clamped.
:::
