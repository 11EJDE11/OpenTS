---
key: HarvesterUnit
summary: UnitTypes the engine treats as a house's harvesters when it counts or replaces them.
see_also: ["system:tiberium", "Harvester"]
when_omitted:
  kind: value
  value: "none"
---

Only the first entry is consulted by the routines that count harvesters: the computer's decision to queue a replacement, the free harvester a unit crate hands a player who owns a refinery and none, the harvester census that spreads a computer house's [weighted patch search](/systems/tiberium/#finding-a-patch) across a field, and the income and truce bookkeeping behind them. The whole list is read in two places: the harvester-truce option shields every listed type from all damage but limpet warheads and keeps it out of every automatic target scan, a human house's included, and recovery from an EMP stun sends any listed vehicle back to harvesting.

:::caution[The truce does not keep a vehicle thief off a harvester]
The test that would refuse the capture cursor is the one place that hands the running vehicle to the check rather than its type, and the two are never equal, so the test can never match. The enter cursor is offered over a harvester whether the option is set or not, and [capture](/systems/capture/) covers what follows.
:::

The list is separate from [`Harvester=yes`](/keys/harvester/#scope-unittype): a type not named here still harvests, it is simply invisible to those counts.

:::danger[The list must not be empty]
Every one of those routines reads the first entry without checking that the list has one.
:::
