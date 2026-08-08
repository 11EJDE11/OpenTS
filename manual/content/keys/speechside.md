---
key: SpeechSide
summary: The side whose voice set narrates a campaign mission.
see_also: [Player, RequiredAddOn]
when_omitted:
  kind: context-dependent
  note: The side implied by the scenario's Player entry, which is GDI when that entry reads exactly GDI and Nod for anything else.
---

```ini title="map file"
[Basic]
Player=Nod
SpeechSide=GDI
```

The voice archive is chosen by the side's position in the rules `[Sides]` list plus one, so the first side draws `SPEECH01.MIX` and the second `SPEECH02.MIX`; any expansion voice packs registered for the same position are layered over it. The setting exists so that a mission fought as one side can be narrated by the other, which is how the stock Firestorm missions are voiced.

It is read only in a campaign mission, and only after the side has already been settled from [`Player`](/keys/player/#scope-scenarios-2), so it overrides that choice for speech alone. Art, interface and the buildable list continue to follow `Player`. Writing `<none>` is the same as leaving the key out.

:::caution[A side with no voice archive stops the mission from loading]
The archive is opened as the mission loads, and the load is abandoned when it cannot be found. The game ships voices for the first two sides only, so naming the `Civilian` or `Mutant` side that the stock rules also declare prevents the mission from starting. A name matching no side at all is worse: it registers a new side, which is then given a position beyond any archive that exists, and the mission fails the same way.
:::
