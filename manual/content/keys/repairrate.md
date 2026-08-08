---
key: RepairRate
summary: The interval between structure repair steps and between self-healing steps.
see_also: ["system:repair"]
when_omitted:
  kind: value
  value: ".016"
---

The value is a fraction of a minute: the engine multiplies it by 900 frames and truncates, giving 14 frames at the default. Both paths test the global frame counter against that interval rather than counting from the moment each object started, so every [structure repair](/systems/repair/#the-repair-tick) and every [self-healing](/systems/repair/#self-healing) object in the match steps on the same frames.

The two paths cannot be timed apart. A service depot has [`URepairRate`](/keys/urepairrate/) and the infantry buildings have [`IRepairRate`](/keys/irepairrate/), but nothing separates a structure under the wrench from an object mending itself.

:::danger[A value below one nine-hundredth crashes the game]
The interval is truncated to a whole number of frames and then used as a divisor. Anything between zero and `1/900` truncates to zero, and the first repairing structure or self-healing object divides by it.
:::
