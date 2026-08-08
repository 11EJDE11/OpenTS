---
key: Weeder
scope: buildingtype
label: Weed refinery
see_also: ["system:ai-base-building"]
when_omitted:
  kind: value
  value: "no"
---

The building accepts a docking request from a `Weeder=yes` UnitType while it holds nothing, and unloads it the way a refinery unloads a Tiberium harvester. A computer house admits such a type to [the base plan it generates](/systems/ai-base-building/#building-the-plan) only while the map carries a veinhole monster, so a map with no veins plans no weed refinery.
