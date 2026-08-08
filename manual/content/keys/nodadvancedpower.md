---
key: NodAdvancedPower
summary: The BuildingType every computer house except one named "GDI" inserts to cover a power shortfall once it can build it.
see_also: ["system:ai-base-building"]
when_omitted:
  kind: value
  value: none
---

The prerequisite test runs against the buildings the house owns at that moment, and failing it falls back to [`NodRegularPower`](/keys/nodregularpower/). Neither the house's tech level nor its [`Owner`](/keys/owner/) bit is consulted, so a type the house could never build itself is still inserted once its prerequisites stand.

:::danger[The prerequisite test needs a type]
This type is read for every non-GDI house that reaches [a power shortfall](/systems/ai-base-building/#power-and-money-interventions), before either branch is chosen. With no type named here the game crashes at that point.
:::
