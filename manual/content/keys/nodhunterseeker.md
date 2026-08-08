---
key: NodHunterSeeker
summary: The type created by a hunter-seeker superweapon fired by any house not acting as GDI.
see_also: [GDIHunterSeeker, HSBuilding, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

This is the fallback half of a two-way branch on the firing house's [`ActsLike=`](/keys/actslike/): only a house acting as GDI takes [`GDIHunterSeeker`](/keys/gdihunterseeker/), and only a house acting as Nod takes this one, which means a HouseType whose section name begins with `Nod` or a scenario section that says so; a neutral house or an added country falls back to acting as GDI. The named type is created as a vehicle at the cell chosen from [`HSBuilding`](/keys/hsbuilding/), facing east, and then [acquires its own target](/systems/superweapons/#hunter-seeker).
