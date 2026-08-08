---
key: FileName
summary: Stem of the artwork file names a tile set loads its tiles from.
see_also: [TilesInSet, SetName, NonMarbleMadness]
when_omitted:
  kind: value
  value: TILE
---

Each tile of the set is looked for as this stem plus a two-digit index counting from `01`, extended with the theater's own suffix. [Theater control files](/formats/theater-control/) covers the lettered alternates built on the same stem and the fallback extension tried when the theater file is missing.

```ini title="TEMPERAT.INI"
[TileSet0631]      ; example set
SetName=Riverbank cliffs
FileName=RVCLIF    ; loads RVCLIF01.TEM through RVCLIF08.TEM
TilesInSet=8
```

Nothing checks that the stem produced a file. A tile whose artwork is absent still becomes a tile type, reports zero width and height, and draws nothing where a cell places it.

:::caution[Only thirteen characters of the path are kept]
A tile type remembers the path it loaded from so that it can read its artwork again after the theater trims it out of memory, and the copy that fills it stops at fourteen bytes of a sixteen-byte record, leaving the two bytes it holds back exactly as the allocation left them. A stem long enough to push the built path to fourteen characters — eight characters for a plain tile, seven for a lettered alternate — writes no terminator, so the name runs on past the record until whatever follows it in memory supplies a zero. The reload opens the file on that run-on name, finds nothing, and hands the tile loader a zero-length buffer that it then reads the set's dimensions out of.
:::
