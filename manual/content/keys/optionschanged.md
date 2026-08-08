---
key: OptionsChanged
summary: Sound played in a modem lobby as the other player's settings arrive.
see_also: [PlayerJoined, PlayerLeft, SystemError]
when_omitted:
  kind: value
  value: none
---

```ini title="rules.ini"
[AudioVisual]
OptionsChanged=OPTCHG ; a sound ID registered in SOUND.INI
```

The sound belongs to the modem and null-modem lobby and is played without a position. The guest hears it each time arriving host options differ from the ones it already holds, and the dialog is refilled with the new map name, preview and accept button; an identical retransmit is silent. The host hears it when the guest's name, color or house changes. Both checks are on the values rather than on the message.

One path outside multiplayer plays it: the main menu sounds it when a typed cheat string is accepted. That path is on the plain menu the game falls back to when the graphical main menu cannot be built, not on the graphical menu itself.
