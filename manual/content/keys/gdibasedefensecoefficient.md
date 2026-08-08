---
key: GDIBaseDefenseCoefficient
summary: Scales how many base defenses every computer house except one named "Nod" plans against its accumulated build cost.
see_also: ["system:ai-base-building"]
when_omitted:
  kind: value
  value: "1"
---

The coefficient applies to every house whose country name is not "Nod", including one named neither side, and it is read only while [the base plan is assembled](/systems/ai-base-building/#building-the-plan). Raising it multiplies the placeholders the plan carries; at `0` the plan reaches its extra defenses without a single interleaved one.
