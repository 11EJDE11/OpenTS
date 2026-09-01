---
key: BaseUnit
summary: The UnitType handed to each house at a multiplayer start and counted as a base of its own.
see_also: ["system:crates", "system:starting-forces", DeploysInto]
when_omitted:
  kind: value
  value: none
---

Nothing about the type itself marks it out; every effect follows from being named here. The one property of the type itself that is read is its [`DeploysInto`](/keys/deploysinto/).

- With bases enabled, each house that is not passive is given one on its start position, or on the nearest cell the [placement search](/systems/starting-forces/#where-an-object-lands) finds within thirty-one cells, and it is the object a house's flag is attached to in capture-the-flag. Enabling bases also takes one off the lobby's unit count to pay for it.
- The random starting units are drawn from the [`AllowedToStartInMultiplayer=yes`](/keys/allowedtostartinmultiplayer/) types with this one held out, and the [average price](/systems/starting-forces/#the-budget) that decides how many are drawn leaves it out too.
- In a short game, a house holding no structures and no unit of this type is defeated.
- A crate collected by a house that has lost its base and can afford to rebuild [delivers one](/systems/crates/#money-and-free-units), and with bases disabled the random vehicle result refuses to hand out this type at all.
- The [Center Base](/commands/centerbase/) command looks for a structure of the type this one [`DeploysInto`](/keys/deploysinto/), and falls back to a unit of this type when it finds no building of the player's at all.

:::danger[Leaving this unset crashes the game]
The value starts as no type at all, and every read above except the two that merely compare pointers goes through it without checking. A multiplayer or skirmish match ends the game while it is still setting up, at the first type allowed to start in multiplayer and before any house is placed. A campaign reaches the value only through the Center Base command.
:::
