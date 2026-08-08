---
key: Priority
scope: sounds
label: Sound playback priority
when_omitted:
  kind: value
  value: "10"
---

There are five sound effect channels. A new sound takes a free one whatever its priority; with all five busy it takes the first channel not playing music whose current priority is lower than its own, cutting that sound off, and is dropped without playing when no channel qualifies. The figure compared is this value scaled by the volume the sound is actually played at, so the same sound competes less well the quieter it is played.
