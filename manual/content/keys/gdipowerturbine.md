---
key: GDIPowerTurbine
summary: The BuildingType a computer house named "GDI" inserts to cover a power shortfall while it owns a power plant with a free upgrade slot.
see_also: ["system:ai-base-building"]
when_omitted:
  kind: value
  value: none
---

The turbine is chosen over [`GDIPowerPlant`](/keys/gdipowerplant/) only while the house owns a `GDIPowerPlant` whose upgrade slots are not full and a draw against [`AIUseTurbineUpgradeProbability`](/keys/aiuseturbineupgradeprobability/) succeeds. The inserted node carries no cell, so [the placement search](/systems/ai-base-building/#choosing-a-spot) resolves where the structure goes. Reaching that branch with no type named here crashes the game.
