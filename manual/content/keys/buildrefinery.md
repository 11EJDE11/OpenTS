---
key: BuildRefinery
summary: The refineries a computer house plans and measures its income against, in order of preference.
see_also: ["system:ai-base-building"]
when_omitted:
  kind: value
  value: ""
---

The refinery enters [the base plan](/systems/ai-base-building/#building-the-plan) through the ordinary candidate scan; the first entry this house may own only marks which queued refinery receives the plan's extra copies, and a refinery the scan never queued receives none. Entry 0 is separately the refinery the money logic counts and prices, both when judging whether a computer house can still earn and while it sells its base back to recover. Entry 0 also decides a crate reward — a crate that would hand out a unit gives a free harvester instead when the house that opened it owns a building of that type and no harvester, a human house included, unless [`UnitCrateType`](/keys/unitcratetype/) names a unit and overrides the pick.

The money check and the crate both read entry 0 without checking that the list has one.
