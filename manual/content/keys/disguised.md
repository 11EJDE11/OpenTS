---
key: Disguised
summary: Hides the soldier behind another type's name, artwork and color, and keeps automatic fire off it.
see_also: [Disguise, "system:target-selection"]
when_omitted:
  kind: value
  value: "no"
---

The identity a disguised soldier shows comes from the InfantryType named by [`Disguise`](/keys/disguise/) in `[General]`, which covers the two ownership tests behind the substituted name and shape. On top of that substitution the soldier is drawn in the local player's own color scheme whenever the local player does not own it, and it is plotted on the radar in the local player's color with no ownership test at all.

The disguise also keeps automatic fire away. A disguised soldier is rejected outright while [a candidate is being scored](/systems/target-selection/#why-a-candidate-is-rejected), so nothing acquires it by scanning. No vehicle choosing on its own whether to run something down will pick one, whoever owns it, and an infantryman routing through a cell a disguised soldier occupies treats that cell as temporarily blocked rather than closed, so it waits rather than going around.

:::caution[Nothing can acquire a disguised soldier on its own]
The rejection reads only the candidate's type. It applies to every scanning object of every house — the disguised soldier's own owner included, and dogs along with everything else, despite a source comment describing an exception for them. A player order, retaliation against a shot the soldier fired, and area damage all still reach it; only the automatic scan does not.
:::
