---
key: DemandLoad
scope: overlaytype
label: Overlay shape
see_also: ["Image", "NewTheater", "Theater"]
when_omitted:
  kind: value
  value: "no"
---

The flag is read after the overlay's shape has already been fetched from the archives under its [Image ID](/keys/image/), so an overlay whose artwork is present is holding it before the setting is consulted. What the flag then skips is the later work: the shape is not fetched again as the theater is set up, and the theater-named copy is not fetched again when a saved game is restored — though the restore does re-attach the archive's copy, which puts the release below back in reach. An overlay that reaches a draw with no shape reads one from disk at that moment and holds it for the rest of the session.

:::caution[Only a theater-aware overlay can be demand loaded]
The deferred read builds its filename only for a [`Theater=yes`](/keys/theater/) or [`NewTheater=yes`](/keys/newtheater/) overlay. On any other overlay no name is built at all: the read is made against a name buffer nothing ever wrote, and the type is left with no artwork of its own to draw.
:::

:::danger[The release hands back memory the type may never have allocated]
The type releases whatever its shape pointer holds when it is destroyed, and again as the theater is set up if it is [`Theater=yes`](/keys/theater/) or [`NewTheater=yes`](/keys/newtheater/). Only a shape this flag actually deferred is a block the type allocated for itself; the pointer left by the earlier fetch belongs to the archive it was read from, and handing that back corrupts the heap: the game may fault there or at a later, unrelated allocation.
:::
