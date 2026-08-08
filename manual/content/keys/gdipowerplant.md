---
key: GDIPowerPlant
summary: The BuildingType a computer house named "GDI" inserts to cover a power shortfall.
see_also: ["system:ai-base-building"]
when_omitted:
  kind: value
  value: none
---

The same type is the one whose free upgrade slot qualifies the house for a [`GDIPowerTurbine`](/keys/gdipowerturbine/) instead; with no such building owned, or with the turbine draw lost, the plant itself is inserted. [Power and money interventions](/systems/ai-base-building/#power-and-money-interventions) covers when that happens. Reaching the insertion with no type named here crashes the game.
