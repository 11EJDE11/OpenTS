---
key: AIBuildsWalls
summary: Lets a computer house close its base plan with the node that runs the perimeter wall planner.
see_also: ["system:ai-base-building"]
when_omitted:
  kind: value
  value: "yes"
---

With `AIBuildsWalls=no` no house appends that node, so no computer base reaches [the wall planner](/systems/ai-base-building/#walls-and-gates), and every house instead receives the extra base-defense placeholders that a house named "Nod" always gets. A house named "Nod" also needs [`NodAIBuildsWalls=yes`](/keys/nodaibuildswalls/) before its wall node is appended.
