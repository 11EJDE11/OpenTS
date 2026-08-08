---
key: NodBaseDefenseCoefficient
summary: Scales how many base defenses a computer house named "Nod" plans against its accumulated build cost.
see_also: ["system:ai-base-building"]
when_omitted:
  kind: value
  value: "1"
---

Only a house whose country name is "Nod" uses this coefficient; every other house takes [`GDIBaseDefenseCoefficient`](/keys/gdibasedefensecoefficient/). It is read only while [the base plan is assembled](/systems/ai-base-building/#building-the-plan), and the extra placeholders a Nod house receives afterwards are not scaled by it.
