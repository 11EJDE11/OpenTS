---
key: Allies
summary: The houses a scenario's house counts as friendly.
see_also: [Owner, AllyReveal, Edge]
when_omitted:
  kind: value
  value: ""
  note: An empty list, which leaves the house allied to nobody but itself.
---

The value is a comma-separated list of house identifiers, matched without regard to letter case against both the section names and the display names of the loaded houses; a name that matches nothing contributes nothing. Only a campaign mission reads its house records, so this is a campaign setting.

```ini title="scenario map file"
[Special] ; a house record in the scenario's own house list
Allies=GDI,Nod
```

An alliance is what keeps two houses from turning on each other: an allied object is [rejected by an automatic target scan](/systems/target-selection/#why-a-candidate-is-rejected), is passed over rather than crushed, and does not provoke [retaliation](/systems/target-selection/#retaliation). A house is always its own ally whether or not it names itself.

:::caution[The alliance runs one way]
Naming a house here makes *this* house treat that one as a friend. It does not make that house treat this one as a friend, and a scenario that wants a mutual truce has to write the pairing into both house records. The check that decides whether every house left alive is allied asks the question in both directions and accepts it only when both agree.
:::

Alliances declared this way are silent. The announcement, the ceasefire sweep that drops targets already acquired, and the [`AllyReveal`](/keys/allyreveal/) map reveal all belong to alliances formed while a scenario is running, and none of them fires while the scenario is being read.
