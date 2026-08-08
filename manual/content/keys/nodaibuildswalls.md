---
key: NodAIBuildsWalls
summary: Lets a computer house named "Nod" close its base plan with the node that runs the perimeter wall planner.
see_also: ["system:ai-base-building"]
when_omitted:
  kind: value
  value: "yes"
---

Only a house whose country name is "Nod" is tested against this setting, and [`AIBuildsWalls=yes`](/keys/aibuildswalls/) is required as well. A Nod house plans its extra base defenses whatever this setting holds, because [that pass](/systems/ai-base-building/#building-the-plan) tests the house name rather than either wall setting.
