---
key: Official
scope: scenarios-2
label: Starting-point selection
when_omitted:
  kind: value
  value: "no"
---

```ini title="map file"
[Basic]
Official=yes
```

In a multiplayer or skirmish game — and not on a generated random map — the same assignment is read again as the scenario finishes loading, and it decides how many starting points the placement pass looks for. Those points are the pool each house draws from: the first house takes one at random, and every house after it takes whichever remaining point lies furthest from the ones already taken.

Marked official, the pass looks for as many points as there are players and computer players together, or for however many of waypoints `0` onward are placed without a gap up to waypoint `7`, whichever number is larger. Left at the default, it looks for exactly eight regardless of who is playing.

Either way, the pass then collects every placed waypoint below that count and makes up any shortfall with random cells on open ground.

:::caution[An unofficial map is always padded to eight]
A two-player map that does not mark itself official still has six extra starting points invented for it on open ground, and the placement pass draws from all eight. Marking the map official is what confines the choice to the waypoints the map designed.
:::

:::danger[A map that cannot host a starting position hangs the load]
Any shortfall between the placed waypoints and the number the pass looks for is made up with random cells on open ground, and the search for each one repeats until it succeeds. On a map where no cell can take a starting position the search never ends and the scenario never finishes loading.
:::
