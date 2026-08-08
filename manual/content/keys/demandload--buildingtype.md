---
key: DemandLoad
scope: buildingtype
label: Structure shape
see_also: ["DemandLoadBuildup", "Image", "Theater"]
when_omitted:
  kind: value
  value: "no"
---

A structure's shape is fetched from the archives twice while the rules are read: once under its [Image ID](/keys/image/), before this setting has been read at all, and once afterwards under the [main-shape basename](/keys/image/#scope-buildingtype). Only the second fetch is skipped. A structure whose two names agree — the ordinary case — is therefore already holding its artwork by the time the flag is consulted, and setting it changes nothing about when the shape is loaded. Deferral takes effect only where the first name resolves to no file.

Where it does take effect, the shape is read from disk the first time something asks to draw a structure of the type, and is then held for the rest of the session. A type nothing ever draws allocates nothing, and a structure whose shape cannot be found is not drawn at all.

The construction animation is a separate setting, [`DemandLoadBuildup`](/keys/demandloadbuildup/). The deploying, door, under-door, bib and Z-shape overlay artwork is fetched with the rules whatever this is set to.

:::danger[The release hands back memory the type may never have allocated]
A type carrying this flag releases whatever its shape pointer holds when the type is destroyed, and again as the theater is set up if the type is also [`Theater=yes`](/keys/theater/) or [`NewTheater=yes`](/keys/newtheater/). Only a shape this flag actually deferred is a block the type allocated for itself; the pointer left by the earlier fetch belongs to the archive it was read from, and handing that back corrupts the heap: the game may fault there or at a later, unrelated allocation.
:::
