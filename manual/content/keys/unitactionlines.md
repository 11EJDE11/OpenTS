---
key: UnitActionLines
summary: Draws a line from each selected object to whatever it has been ordered at.
see_also: [ToolTips, SidebarCameoText]
when_omitted:
  kind: value
  value: "yes"
---

The line is drawn once per rendered frame, for every selected object that belongs to a house under the player's own control; a selected object belonging to anyone else never draws one, whatever the setting. The line reports the order the object is carrying — its movement destination, its target, or the building it is heading for — and it is drawn over the tactical map only, after the objects themselves.

The in-game game controls dialog carries the same switch and writes the choice back to `sun.ini`. Changing it there takes effect on the next frame rather than at the next scenario.
