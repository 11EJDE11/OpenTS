---
title: Decode Move to Cell on the width its file declares
category: fix
release: 0.1.0
targets:
- type: mission
  id: TMISSION_MOVECELL
  effect: changed
- type: key
  id: NewINIFormat
  effect: changed
credit: [ZivDero]
---

A Move to Cell script line now reads its cell on the width the scenario declares, so a line
written for a Tiberian Sun map reaches the cell it names. The split was always made on the
old width of 128 columns, which no map wider than that can express, so every such line on a
modern map landed somewhere else entirely or off the map.

The conversion happens as the line is read rather than as the team acts on it, because the
scenario's declared width is not carried into a save game and so is no longer available once
one is loaded. A number that decodes to a cell outside the map now leaves the line without a
target instead of aiming the team at a placeholder cell.
