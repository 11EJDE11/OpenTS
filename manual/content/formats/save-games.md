---
format_id: save-games
title: Save games
summary: Stores versioned OpenTS game state in `.SAV` compound-document files.
kind: binary
extensions:
  - .SAV
role: persistence
source_files:
  - code/loaddlg.cpp
  - code/saveload.cpp
  - code/savever.cpp
  - code/abstract.cpp
  - code/objtype.cpp
  - code/unittype.cpp
---

The save dialog creates `.SAV` files. Each file is an OLE compound document: the listing details live in the document's own property set, and the game state goes into a single `CONTENTS` stream that is compressed as it is written.

The dialog names a new save `SAVE` followed by four hexadecimal digits, drawing again until it finds a name no existing file answers to; saving over a listed game reuses that game's name. A multiplayer save is written under one fixed name instead and is never offered in the list.

## What the file holds

The property set carries the description shown in the list, the player's name and house, the campaign and scenario numbers, the game type, three timestamps, the name of the program that wrote the save, and two version stamps — the save format's own version and the build version of the game that wrote it.

The `CONTENTS` stream is a fixed sequence of records — the scenario, the environment, the rules, the map, the loose global values, and every list of type definitions and runtime objects — written and read back in the same order. Each list stores its own length ahead of its members, and each member is restored by creating an empty object of the right kind and reading the stored bytes straight over it. What a save holds is therefore a copy of each object as it stood in memory rather than a field-by-field record of it. Type definitions travel with the save, so a save carries the rules types it was made with rather than looking them up again on load. Artwork does not travel with it: a restored type has its shape and voxel pointers released and cleared, then fetches from the archives again, so a save loaded against a changed set of files gets the current artwork. One piece does not come back. A UnitType drawn from shapes is given a [voxel turret](/formats/vxl-hva/) when the rules are read, by a routine no restore calls; the restore takes the ordinary voxel path instead, which releases that turret along with the body model it could not find. Its voxel barrel is fetched back, and the barrel is what the shape path draws.

## What is checked

The build version stamp decides whether a file is offered at all. The load dialog reads the property set of every `.SAV` in the game directory and skips any file whose stamp is neither the running build's nor the Tiberian Sun release's. A file left over from the Tiberian Sun release is listed with a leading `*`, and loading it puts the engine into a mode where every kind of object that has grown since reads back only as many bytes as it used to occupy.

Beyond that stamp and the add-on the scenario declares, nothing about a save is measured against the game it is being loaded into. A save made under one set of rules and loaded under another is not detected, and the type definitions stored in the file are simply restored over the ones the rules built.

:::caution[A load that stops partway still reports success]
Reading `CONTENTS` clears the scenario before it starts and gives up at the first record it cannot restore, but its result is discarded and the load reports that it succeeded. A save whose scenario needs an add-on that is not installed stops at the first record for that reason, and the game carries on into a scenario that was cleared and never refilled.
:::
