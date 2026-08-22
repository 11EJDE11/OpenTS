---
title: Aircraft operations
summary: "Coordinates aircraft destinations, landing zones and the conflicts that can prevent a landing."
category: units-movement
keys: []
related:
  - type: system
    id: repair
---

Aircraft use their destination as a landing-zone reservation while they approach a cell. The reservation is not an occupant: it prevents another friendly aircraft from choosing the same landing zone before either one arrives.

## Landing-zone conflicts

Inside the playable area, a landing-zone scan rejects a cell when a foot object is already present. The aircraft performing the scan does not block itself unless the caller requests a strict check, and a carryall does not treat the vehicle it is collecting as a blocker. An actual occupant blocks the cell regardless of who owns it or how the houses are allied.

A destination held by another active aircraft also blocks the cell in either of these cases:

- both aircraft belong to the same house;
- each aircraft's house considers the other aircraft allied.

A hostile aircraft's destination does not reserve the cell, and neither does a one-way alliance. The aircraft may still be unable to land there for another reason, and an aircraft that actually reaches the cell blocks it like any other occupant.
