---
key: PadAircraft
summary: The AircraftTypes bundled into the price of the pad they dock at.
see_also: ["system:production"]
when_omitted:
  kind: value
  value: ""
---

The structure that entry 0 names as its first [`Dock=`](/keys/dock/) target treats the average price of entries 0 and 1 as a bundled share of its own [`Cost=`](/keys/cost/#scope-aircrafttype). The price asked for the structure adds that share back, so what it costs to buy, what selling it refunds and what destroying it is worth are all the written figure; the share is stripped out of its repair bill alone. Separately, a [`HoverPad=yes`](/keys/hoverpad/) structure that was built rather than captured receives one aircraft of entry 0 as it opens, docked and on guard. [`SeparateAircraft=yes`](/keys/separateaircraft/) removes both the bundled price and the free aircraft.

The two structures need not be the same one: the price is charged to whatever entry 0 docks at, and the aircraft is handed to whatever carries `HoverPad=yes`.

The engine reads entries 0 and 1, and the first entry's first dock, without checking that they exist.

:::danger[An empty list crashes the game before a match starts]
When no rules layer sets this key the list is empty, and the first building price the game works out — the cost under any structure cameo, or the computer's first refinery estimate — reads its first entry through a pointer to nothing and the game crashes. [`SeparateAircraft=yes`](/keys/separateaircraft/) does not prevent it: the list is read before that setting is consulted. Name at least one AircraftType here.
:::
