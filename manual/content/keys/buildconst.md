---
key: BuildConst
summary: The construction yard BuildingType; only the first entry is read.
see_also: ["system:ai-base-building"]
when_omitted:
  kind: value
  value: ""
---

Entry 0 stands for the construction yard wherever the engine needs one, and entries after it are read by nothing. Only a building of that exact type counts towards a house's construction yards, so a computer house that owns none builds no structures at all and [`ConstructionYard=yes`](/keys/constructionyard/) on another type does not stand in for it. While [the base plan](/systems/ai-base-building/#building-the-plan) is assembled, a [`Prerequisite`](/keys/prerequisite/) naming that type counts as already met. A UnitType whose [`DeploysInto`](/keys/deploysinto/) names it is the MCV a base-building computer house acts on: outside a campaign game it sends one out to hunt while it owns no construction yard, and it deploys one standing guard instead of leaving it there.

The engine reads entry 0 without checking that the list has one.

:::caution[Building lists are split on commas alone]
Names are matched without regard to case, and while the value is trimmed at its ends, the split is on commas alone, so `GAPOWR, NAPOWR` looks for a type whose ID begins with a space. A name matching no BuildingType ID — an existing UnitType or InfantryType ID included — adds a new BuildingType carrying nothing but that name, which no house may own, instead of reporting a problem, and only the first 127 characters of the value are read, so a long list loses its tail the same way. Every building list in `[AI]` is read like this, and a scenario carrying its own `[AI]` section replaces each list it names.
:::
