---
key: GDIWallDefenseCoefficient
summary: The difficulty-scaled part of the cap on wall tower and base defense pairs a computer house named "GDI" appends after planning its perimeter wall.
see_also: ["system:ai-base-building"]
when_omitted:
  kind: value
  value: "3"
---

The coefficient is multiplied by `3` minus the house's difficulty slot, and the product is added to [`GDIWallDefense`](/keys/gdiwalldefense/), so it is what makes an advantaged computer house ring its wall with more defenses than a penalized one; [Walls and gates](/systems/ai-base-building/#walls-and-gates) gives the full expression. Only a house whose country name is "GDI" appends those pairs.
