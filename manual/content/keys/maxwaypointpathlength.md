---
key: MaxWaypointPathLength
summary: Number of markers a player may place into one waypoint path.
when_omitted:
  kind: value
  value: "15"
---

A path already holding this many markers takes no more. The cursor over an empty cell turns into the refusal cursor, and placing the marker that reaches the count also drops the player out of [Waypoint Mode](/commands/waypointmode/) rather than leaving it running with nothing left to do. The same limit closes the shift-click that loops the selected path.

The count is not the only thing that closes a path: one that is already being traveled refuses further markers whatever its length.
