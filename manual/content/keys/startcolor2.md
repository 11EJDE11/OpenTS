---
key: StartColor2
summary: The other end of the color range a spark or railgun particle is created at.
see_also: ["StartColor1", "ColorList", "ColorSpeed"]
when_omitted:
  kind: value
  value: 0,0,0
---

The second end of the range described by [`StartColor1`](/keys/startcolor1/), which covers how the two are read, how the starting color is picked between them, and what leaving both black does. Setting only one of the pair narrows the range to the span between that color and black, since the unset end stays at `0,0,0`.

:::caution[A partial triplet leaves the remaining channels unpredictable]
`StartColor2=255` puts the far end of the range at a color whose green and blue come from whatever was last in that storage; [INI syntax](/formats/ini-syntax/#malformed-values) explains the incomplete-value defect behind it, and why leaving the key out is safe.
:::
