---
key: WallTower
summary: The BuildingType that joins a brick or sandbag wall run from any direction.
see_also: ["system:walls-and-gates", "system:ai-base-building"]
when_omitted:
  kind: value
  value: none
---

Everything the wall tower does follows from being the type this key names; there is no flag for it. While it is standing and undestroyed, the [connection logic](/systems/walls-and-gates/#connection-frames) reads it as a continuation of a brick or sandbag wall from all four directions at once, which is what lets a wall run turn a corner or meet another run through it. Nod wall is a separate family and never connects to it. For a house a human is playing, placing one on a brick or sandbag wall the same house owns removes that wall first, quietly and without refund, while a computer house's tower is placed on top of the surviving segment; and the tower may be placed on an undamaged segment, which no ordinary wall building may do. Once placed it forces its four cardinal neighbors to rebuild their connection frames.

:::caution[Removing a tower damages the wall around it]
When the tower is taken off the map its four cardinal neighbors rebuild their frames, and each of those still holding an undamaged wall is then hit for 200 damage. Any wall whose [`Strength`](/keys/strength/#scope-overlaytype) is 200 or below therefore loses a stage on every side of the tower, which can start the [cascade](/systems/walls-and-gates/#stepping-through-the-stages) along the run.
:::

A node for this type precedes every base-defense placeholder a GDI plan carries, and once the tower is placed the defense node behind it moves onto the tower's cell. A tower the house already owns is left out of the buildings [defense candidates](/systems/ai-base-building/#base-defenses) have their prerequisites tested against, but for a GDI house the type is put back into that list unconditionally, so a GDI defense that names it as a prerequisite is buildable before any tower stands.

:::danger[Every base-building computer house reads this type]
A base-defense node the planner cannot fill is compared against this type before it is deleted, whatever the house is named, and a house named "GDI" reads it again while laying its plan and its wall. With no type named here the game crashes on whichever of those paths runs first — for GDI, already while laying the plan.
:::
