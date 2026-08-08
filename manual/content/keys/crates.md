---
key: Crates
summary: Starting state of the crates option for a multiplayer or skirmish match.
see_also: ["system:crates"]
when_omitted:
  kind: value
  value: "yes"
---

The value seeds the match's crate option when a session begins, and the game setup screen then writes over it. The starting placement pass and the expiry sweep both read the resulting match option, not this setting.

:::caution[One crate path reads this setting directly]
The replacement crate placed after a crate is collected consults this key rather than the match option, so the two can disagree. `Crates=yes` in the rules with crates switched off for the match still produces a replacement whenever a crate dropped by a destroyed [`CarriesCrate=yes`](/keys/carriescrate/) vehicle is collected, and `Crates=no` in the rules with crates switched on suppresses that replacement while the expiry sweep keeps running.
:::
