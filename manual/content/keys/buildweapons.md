---
key: BuildWeapons
summary: The war factories a computer house resolves a generic factory prerequisite to, in order of preference.
see_also: ["system:ai-base-building"]
when_omitted:
  kind: value
  value: ""
---

The first entry this house may own is what a [`Prerequisite=FACTORY`](/keys/prerequisite/) resolves to while [the base plan](/systems/ai-base-building/#building-the-plan) is assembled, and it is also moved to second place in the candidate list. The whole list answers whether the house owns a factory at all, which feeds both the check on whether it can still earn and whether it keeps producing units while low on credits; entry 0 supplies the factory price that first check prices a replacement at, and entries 0 and 1 together decide whether a house selling its base back can afford a harvester instead of a refinery.

The engine reads entries 0 and 1 without checking that the list has them, so a list of fewer than two entries is read past its end.
