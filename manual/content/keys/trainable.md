---
key: Trainable
summary: Allows an object of this type to accumulate experience from the kills it scores.
see_also: ["system:veterancy"]
when_omitted:
  kind: context-dependent
  note: "`yes` for AircraftTypes, InfantryTypes, and UnitTypes; BuildingTypes start at `no` and must set the key to earn from their own kills."
---

The test is made on the type that dealt the fatal damage, at the moment the victim dies. A type set to `no` never accumulates anything, however much it destroys.

The key gates earning only. An object that was given a rank some other way — by deploying from a promoted vehicle, by a veterancy crate, by a reinforcement's [`VeteranLevel`](/keys/veteranlevel/), or by an armory — holds that rank and everything it unlocks regardless of this setting. Setting `Trainable=yes` on a BuildingType is what lets a defensive structure climb the ranks by its own shooting.
