---
key: House
summary: The country whose house in the match owns teams built from this TeamType.
see_also: [TaskForce, Script, Waypoint, "system:ai-team-production"]
when_omitted:
  kind: value
  value: none
---

The value names a country, matched against every registered country's ID and its [`Name=`](/keys/name/) string, and the team is given whichever house in the match is playing it. A country no house is playing resolves to nothing, and so does `<none>` — an unrecognized name is not rejected, it registers a fresh country under that name, which by definition nobody is playing. Omitting the key keeps whatever an earlier layer set.

```ini title="ai.ini or map file"
[MyRaidTeam] ; example TeamType
Name=Raid team
House=Nod
TaskForce=MyRaidForce ; defined under [TaskForces]
Script=MyRaidScript   ; defined under [ScriptTypes]
```

The owner settles who the team's members are created for and who they may be recruited from — a candidate of any other house is refused outright — and it is the house the team counts against for the defensive-team census and the per-house team limit.

:::caution[An AI trigger ignores this setting]
A team raised through the [AI trigger pass](/systems/ai-team-production/#from-suggestion-to-team) is created for the house whose pass raised it, whatever country this key names. The setting decides the owner only where a team is created from the TeamType alone: the [Reinforcement (team)](/mapping/actions/taction-reinforcements/) and [Create Team](/mapping/actions/taction-create-team/) trigger actions. A [Change team](/mapping/missions/tmission-teamchange/) mission hands the new team the old team's house instead.
:::

:::danger[A team with no owner faults on its first logic pass]
Both paths that read this key go ahead whether or not it resolved, and neither the delivery routine nor the team logic checks that an owner is there. A reinforcement faults where it reads the owner's map edge to bring the group in; a team from the Create Team action faults on its first logic pass, where the recruitment gate reads the owner. A misspelled country name, or a country the rules register but no house in this match is playing, is enough to reach either.
:::
