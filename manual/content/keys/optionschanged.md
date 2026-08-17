---
key: OptionsChanged
summary: Sound played when the main menu accepts a typed cheat string.
see_also: [PlayerJoined, PlayerLeft, SystemError]
when_omitted:
  kind: value
  value: none
---

```ini title="rules.ini"
[AudioVisual]
OptionsChanged=OPTCHG ; a sound ID registered in SOUND.INI
```

One path plays the sound, and it is not a multiplayer one: the main menu sounds it when a typed cheat string is accepted. That path is on the plain menu the game falls back to when the graphical main menu cannot be built, not on the graphical menu itself, so a run that builds its menu never reaches the sound at all. [Developer mode and diagnostics](/systems/developer-mode/#the-main-menu-code-recognizer) covers the strings the menu recognizes.

The sound answered the multiplayer lobby it is named for, where each side heard it as the other's settings arrived. That lobby belonged to modem and null-modem play and went with them.
