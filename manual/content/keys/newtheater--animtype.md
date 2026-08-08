---
key: NewTheater
scope: animtype
label: Theater animation naming
see_also: ["Theater", "Image"]
when_omitted:
  kind: value
  value: "no"
---

The file keeps its `.SHP` extension and the second letter of its name is rewritten to match the theater — `T` in temperate, `A` in snow — provided the name starts with `GA`, `GT`, `NA`, `NT`, `CA`, or `CT`, compared without regard to case. Any other name is left as written.

Unlike [`Theater=yes`](/keys/theater/#scope-animtype), this path does honor the animation's [`Image=`](/keys/image/#scope-animtype) and rewrites that name rather than the AnimType ID. The AnimType ID is used only when no Image ID is set.

:::caution[Artwork already in hand is not replaced]
The rename is reached only while the animation still has no shape. One that already found a file under its unrewritten name keeps it, so providing that name alongside the theater variants leaves the variants unused. The animation is loaded under its plain name as soon as its type is created, so a type whose unrewritten file exists never reaches the rename at all — and one that did load a theater variant keeps that variant when the scenario theater later changes.
:::
