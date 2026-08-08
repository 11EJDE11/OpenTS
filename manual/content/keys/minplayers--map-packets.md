---
key: MinPlayers
scope: map-packets
label: Packet-listed player limits
see_also: [Description, CD]
no_effect: true
when_omitted:
  kind: value
  value: "2"
---

```ini title="MyMaps.PKT"
[MYMAP] ; a section named by the packet's [MultiMaps] list
MinPlayers=2
```

The value is read into the entry's minimum player count and then, from the same assignment, into its maximum. Both counts are private to the listing and nothing reads either one afterwards, so neither the lobby, the starting-position pass nor the house assignment consults them: a map declaring `MinPlayers=2` can still be started with eight players.

There is no `MaxPlayers` assignment. The read that fills the maximum names this key as well, so the maximum a listing carries can only ever equal its minimum, and the maximum's own initial value of `4` survives only when this assignment is absent.
