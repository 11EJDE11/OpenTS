---
key: GenericBeep
summary: Sound acknowledging a volume slider move and a moderation action in the online lobby.
see_also: [GenericClick, SpeakDelay]
when_omitted:
  kind: value
  value: none
---

```ini title="rules.ini"
[AudioVisual]
GenericBeep=BEEP1 ; a sound ID registered in SOUND.INI
```

Three volume settings play it as feedback while they are dragged, each as feedback on the new setting and each with its own exception. The music volume plays it only while no music is actually playing, so a slider moved during a score is heard against the score instead. The sound effect volume plays it every time. The speech volume plays it only outside a running game; inside one it speaks a random taunt instead, and only when nothing else is already speaking.

The Westwood Online lobby plays it once as a squelch, kick or ban is applied to the current selection in the user list, both in the main lobby window and in the game options window. An action attempted with nothing selected makes no sound.
