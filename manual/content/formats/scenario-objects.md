---
format_id: scenario-objects
title: Scenario object records
summary: Defines the vehicles, infantry, aircraft, structures, and trigger ownership loaded with a scenario.
kind: file
source_files:
- code/unit.cpp
- code/infantry.cpp
- code/aircraft.cpp
- code/building.cpp
- code/trigtype.cpp
- code/tagtype.cpp
- code/house.cpp
filenames:
- "<scenario>.INI"
- "*.MAP"
- "*.MPR"
related:
- type: format
  id: ini-syntax
---

Scenario INI files store placed objects as comma-separated entries in `[Units]`, `[Infantry]`, `[Aircraft]`, and `[Structures]`. Every row begins with a HouseType ID, an ObjectType ID, and strength; the remaining positional fields carry the location, facing, mission, tag, and type-specific state. The section entry name identifies the row when the engine writes a scenario, while the loader processes rows in their section order.

## Runtime owners

A HouseType definition and a live house are different things. Rules can register a HouseType that the current campaign or multiplayer session never instantiates. A vehicle, infantry, aircraft, or structure row is created only when its first field names a registered HouseType with a live house in the current session. An unknown owner or a registered owner with no live house skips the row before an object is allocated; the type, location, and remaining fields are not used. Other validation, including whether the object can be placed at its requested location, still applies afterwards.

Trigger definitions in `[Triggers]` use the same live-owner rule. The legacy `<none>` owner selects the first registered house type and is accepted only when that house has a live instance. Trigger names are registered before their bodies are read so links may point forward, but a definition whose owner does not resolve is deleted. A link naming a missing or rejected definition remains empty, and a `[Tags]` row naming one receives no trigger and remains inert. Owner names are resolved only through registered HouseTypes and current houses; the loader does not interpret stock-side names or `Spawn1`-style aliases specially.

## Vehicle follower IDs

The follower field after a `[Units]` row's bridge flag is the zero-based source-row position of the vehicle that follows it, or `-1` for no follower. OpenTS resolves these links only after every unit row has been considered. Each successfully placed vehicle remains mapped to its original section position, so rejecting an earlier row does not renumber the rows after it. A follower position that is negative, outside the section, skipped for its owner or type, or rejected during placement produces no link; it is never redirected to whichever vehicle happened to occupy the compacted runtime list position.
