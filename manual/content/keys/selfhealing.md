---
key: SelfHealing
summary: Mends the object one strength point at a time while it is at or below the yellow damage line.
see_also: ["system:repair"]
when_omitted:
  kind: value
  value: "no"
---

The tick runs on [`RepairRate`](/keys/repairrate/) — 14 frames at the default — and restores exactly one strength point, a figure no setting changes. It costs nothing, needs no building and no order, and works the same way on structures, vehicles, aircraft and infantry.

Granting the `SELF_HEAL` ability through [`VeteranAbilities`](/keys/veteranabilities/) or [`EliteAbilities`](/keys/eliteabilities/) reaches the same path, so a promoted object heals exactly as a type with this flag does.

:::caution[Healing stops at the yellow line, not at full strength]
The tick is refused the moment the object rises above [`ConditionYellow`](/keys/conditionyellow/), so it ends one point past that threshold. At the engine default a self-healing object recovers to just over half strength and stops; [`TiberiumHeal=yes`](/keys/tiberiumheal/#scope-aircrafttype) is the flag that heals an object all the way.
:::
