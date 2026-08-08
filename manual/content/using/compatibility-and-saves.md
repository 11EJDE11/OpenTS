---
title: Compatibility and save games
summary: How OpenTS lists and loads save games stamped by the vanilla game or the current build.
category: compatibility-migration
source_files:
  - README.md
  - code/loaddlg.cpp
  - code/savever.cpp
related:
  - type: using
    id: project-status
---

OpenTS uses the English Tiberian Sun 2.03 release as its inherited data and behavior baseline.

Every save file's header carries an internal version stamp. The load dialog lists a file when all of these hold, tested in this order:

- it is not the multiplayer network save file, which never appears in the list;
- its header record can be opened and read;
- **Any of:** its stamp is the one vanilla Tiberian Sun writes, or its stamp is the one the current build writes.

The stamp the current build writes is fixed in the executable, and it is not the vanilla one, so a vanilla-stamped save is always listed with a `*` before its description and a save this build made never carries one.

Loading a `*` entry switches the engine to its old-save path: the save header is parsed in the older layout when needed, and objects are read using the older layouts that vanilla saves store.
