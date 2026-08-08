---
title: Base placement and adjacency
summary: "Determines whether a pending building placement is adjacent to an eligible owned building."
category: buildings-economy
keys:
  - Adjacent
  - BaseNormal
---

The proximity check applies to a pending BuildingType placement with a valid foundation, and only to the local player's own house. It scans the cells around that foundation for an eligible anchor owned by the same house; a placement for any other house is not scanned at all and passes. Nor is one scanned in the map editor, where the check passes for every house, the local player's included.

## Anchor eligibility

Two settings decide the check, and the table sets them against the object each is read from: one comes from what already stands on the map, the other from what is being placed. Neither is a radius the existing building projects, so changing `Adjacent` on a BuildingType moves where that type may be placed and leaves every other type where it was.

| Setting | Read from | What it controls |
| --- | --- | --- |
| [`BaseNormal`](/keys/basenormal/) | The type of a building already on the map | Whether a building of that type may serve as an anchor |
| [`Adjacent`](/keys/adjacent/) | The BuildingType being placed | How far its pending foundation searches for an anchor |

```ini title="rules.ini"
[GAPOWR]
BaseNormal=no ; placed instances cannot anchor later placements
Adjacent=5   ; pending instances use this search distance
```

## Placement decision order

1. Read the pending BuildingType's foundation dimensions and add one cell to its `Adjacent` value to form the scan area.
2. Skip cells covered by the pending foundation.
3. Accept a wall placement when a scanned cell is owned by the same house, whether or not a building stands in that cell.
4. Accept any placement, a wall's included, when a scanned cell holds a building owned by the same house whose type has `BaseNormal=yes`.
5. Reject the placement when no scanned cell satisfies either test.

:::note[Adjacent zero still permits contact]
The scan adds one cell to the stored value. `Adjacent=0` can therefore find an eligible anchor touching the pending foundation.
:::
