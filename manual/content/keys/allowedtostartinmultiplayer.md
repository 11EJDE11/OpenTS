---
key: AllowedToStartInMultiplayer
summary: Lets a vehicle or infantry type appear among the units each house is handed at a multiplayer start.
see_also: [BaseUnit, Cost, TechLevel, Owner]
when_omitted:
  kind: value
  value: "yes"
---

```ini title="rules.ini"
[MYSUPERTANK] ; a UnitType registered in [VehicleTypes]
AllowedToStartInMultiplayer=no
```

Only a UnitType's and an InfantryType's value is read; a BuildingType or AircraftType stores it and is never asked. The pass that reads it runs once, as a multiplayer or skirmish scenario finishes loading, and never in a campaign.

The setting is spent twice in that pass. First, every allowed vehicle except the [`BaseUnit`](/keys/baseunit/) and every allowed infantry type contributes its [`Cost`](/keys/cost/#scope-aircrafttype) to one game-wide average price. Each house is then given a budget of that average multiplied by the lobby's unit count, one less than the lobby figure when bases are enabled, because the base unit is paid for out of it.

Second, each house builds its own shortlist from the same allowed types, keeping only those its [`TechLevel`](/keys/techlevel/#scope-aircrafttype) reaches and its [`Owner`](/keys/owner/) admits. The budget is then spent one object at a time, at that object's own cost: a vehicle drawn at random while the spend is still under two thirds of the budget, and an infantry type after that. Denying a type therefore does two separate things — it removes the type from every shortlist, and it moves the average price that decides how many objects everybody gets.

:::danger[Something must remain allowed]
The average price divides by the number of allowed types with no zero check, so denying every InfantryType and every UnitType but the base unit divides by zero as the match sets up. Denying every InfantryType alone is the same hazard by another route: once a house has spent two thirds of its budget the vehicle branch closes and only infantry may be drawn, so the spending loop calls through a type it never picked. A house whose own shortlist comes out empty while it still has budget fails the same way.
:::
