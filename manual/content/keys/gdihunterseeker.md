---
key: GDIHunterSeeker
summary: The type created by a hunter-seeker superweapon fired by a house acting as GDI.
see_also: [NodHunterSeeker, HSBuilding, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

The choice is made from the firing house's [`ActsLike=`](/keys/actslike/), which defaults to GDI for a HouseType whose section name begins with `GDI` and is overridden per scenario. Every other house gets [`NodHunterSeeker`](/keys/nodhunterseeker/) instead; there is no third branch and no per-house key. The named type is created as a vehicle at the cell chosen from [`HSBuilding`](/keys/hsbuilding/), facing east, and then [acquires its own target](/systems/superweapons/#hunter-seeker).
