---
key: DemandLoad
scope: animtype
label: Animation shape
see_also: ["FreeAfterPlaying", "Image", "NewTheater", "Theater"]
when_omitted:
  kind: value
  value: "no"
---

The flag is read after the animation's shape has already been fetched from the archives under its [Image ID](/keys/image/#scope-animtype), so an animation whose artwork is present is holding it before the setting is consulted and nothing is deferred. What the flag then skips is the later work: the shape is not fetched again when the theater is set up, and the theater's own copy is not fetched again when a saved game is restored — though the restore does re-attach the archive's copy, which puts the release below back in reach.

An animation that reaches a draw with no shape reads one from disk at that moment, and the frame count and loop end the type left unset are taken from it then rather than at load time. The name built for that read is the Image ID if the animation has one and the AnimType ID otherwise, with a `.SHP` extension; a [`Theater=yes`](/keys/theater/#scope-animtype) animation instead uses the AnimType ID with the theater's own extension, dropping the Image ID, and a [`NewTheater=yes`](/keys/newtheater/#scope-animtype) one has the built name rewritten for the theater.

On a `Theater=yes` or `NewTheater=yes` animation the shape the type is holding is released as the theater is set up rather than replaced. It is released with the type as well, and on a [`FreeAfterPlaying=yes`](/keys/freeafterplaying/) animation as soon as the animation finishes playing — the only combination that gives artwork back during a match.

:::danger[The release hands back memory the type may never have allocated]
Both releases run on whatever the shape pointer holds. Artwork the animation fetched for itself is a block it may give back; the pointer left by the earlier archive fetch is not, and handing that back corrupts the heap: the game may fault there or at a later, unrelated allocation.
:::
