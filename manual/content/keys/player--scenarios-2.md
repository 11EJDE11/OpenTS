---
key: Player
scope: scenarios-2
label: Presentation side
see_also: [SpeechSide, Theater]
when_omitted:
  kind: value
  value: "GDI"
---

```ini title="map file"
[Basic]
Player=Nod
```

Long before the house is resolved, the same assignment is read as plain text and compared against `GDI`, ignoring case. An exact match prepares the mission for the first side; every other value, a valid house name included, prepares it for the second. That choice selects the side's artwork, interface and sidebar, and seeds the voice set before [`SpeechSide`](/keys/speechside/) gets its chance to change it.

Because this is a text comparison rather than a house lookup, a mission played as a house whose ID is neither `GDI` nor a Nod house is still presented as Nod. The comparison is skipped entirely outside a campaign, where the side comes from the lobby.

:::caution[A side whose files cannot be prepared abandons the load]
Preparing the side mounts its artwork and interface archives, and the scenario load is given up when that fails. The same applies to the voice set prepared immediately afterwards.
:::
