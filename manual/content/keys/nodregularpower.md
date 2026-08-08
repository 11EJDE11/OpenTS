---
key: NodRegularPower
summary: The BuildingType every computer house except one named "GDI" inserts to cover a power shortfall.
see_also: ["system:ai-base-building"]
when_omitted:
  kind: value
  value: none
---

It is taken whenever the buildings the house owns do not yet meet the prerequisites of [`NodAdvancedPower`](/keys/nodadvancedpower/), and a house named neither "GDI" nor "Nod" answers its shortfalls from this pair as well. Reaching that fallback with no type named here crashes the game.
