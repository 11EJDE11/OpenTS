---
key: Volume
summary: The share of full loudness the sound effect is played at.
see_also: [Priority, SoundVolume, VoiceVolume]
when_omitted:
  kind: value
  value: "1.0"
---

The value scales whatever loudness the caller asked for. Nothing narrows the figure as it is stored, and the product is clamped at `1.0`; a figure above `1.0` therefore raises the requested loudness rather than lowering it, up to that ceiling. Both paths that play a sample apply it. An ordinary sound effect is scaled by this and by the player's [`SoundVolume`](/keys/soundvolume/) setting, and falls silent altogether when that setting is at zero; the voice path applies this alone and leaves the rest to the caller.

```ini title="sound01.ini"
[SCOLD8]
Priority=75
Volume=0.5
```

The product is also what [`Priority`](/keys/priority/#scope-sounds) is scaled by before the channels are compared, so lowering a sound's volume also makes it easier to cut off.
