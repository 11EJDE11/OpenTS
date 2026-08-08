---
key: SoundLatency
summary: How far ahead of the picture a movie's sound is assumed to be running, in sixtieths of a second.
see_also: [SoundVolume, StretchMovies]
when_omitted:
  kind: value
  value: "9"
---

A movie keeps its video in step with its audio by asking where the sound has reached and drawing to match. This figure is subtracted from that answer, so a larger figure holds the picture further back; the answer is floored at zero, so a figure larger than the sound has actually played simply pins the picture at the start.

The adjustment is thrown away unless DirectSound reports the sound device as an emulated driver. On a device with real hardware mixing it is zeroed as the movie player starts, and the figure has no effect at all.

The setting has no control on any options screen, but saving the options writes it back to `sun.ini` with the rest.
