---
key: RadialColor
summary: The color of the radius ring a selected structure draws on the tactical map.
see_also: [HasRadialIndicator, "system:cloaking"]
---

Three comma-separated channel values from `0` to `255`.

:::caution[A partial triplet leaves the remaining channels unpredictable]
`RadialColor=255` draws a ring whose green and blue come from whatever was last in that storage; [INI syntax](/formats/ini-syntax/#malformed-values) explains the incomplete-value defect behind it, and why leaving the key out is safe.
:::
