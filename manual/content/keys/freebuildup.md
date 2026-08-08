---
key: FreeBuildup
summary: Releases the structure's construction artwork once it is no longer needed.
see_also: ["Buildup", "DemandLoadBuildup"]
when_omitted:
  kind: value
  value: "no"
---

With the flag set, the construction artwork is released at three moments: as each structure of the type is created and has been asked whether it may ever be sold, as a structure of the type finishes its buildup and opens, and as a structure of the type is taken off the map. Only [`DemandLoadBuildup=yes`](/keys/demandloadbuildup/) fetches it again afterwards.

:::danger[On its own the release hands back memory the type never allocated]
Without `DemandLoadBuildup=yes` the artwork was read straight out of the archive that holds it, and what is released is a pointer into that archive's own block. Handing it back corrupts the heap, and the game may fault there or at a later, unrelated allocation. Nothing reloads the artwork either, so every structure of the type is drawn as nothing while it builds, and every one after the first is marked unsellable as well. Set the two together or leave both off.
:::
