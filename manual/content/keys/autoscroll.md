---
key: AutoScroll
summary: Parsed scrolling flag that the engine never uses.
no_effect: true
see_also: [ScrollRate, ScrollMethod]
when_omitted:
  kind: value
  value: "yes"
---

The name promises control over the map scrolling by itself while the pointer rests against the edge of the screen. That scrolling happens either way: the tactical map keeps its own switch for it, that switch starts out on, and nothing ever turns it off or connects it to this flag. Saving the options writes the flag back out with whatever it was read as, and no gameplay path reads it in between.
