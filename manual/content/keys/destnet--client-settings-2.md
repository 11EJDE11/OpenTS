---
key: DestNet
scope: client-settings-2
label: Bridged address read at startup
see_also: [Socket, NetCard]
when_omitted:
  kind: value
  value: ""
---

This is the earlier of the two reads of the assignment, made before the display is prepared, and it parses the value differently from the read that follows. The parse begins at the ninth character of the value rather than the first, so the leading eight characters are dropped: a value shorter than that is read as empty and does nothing, and a longer one contributes only what is left after those eight.

Whatever address this read arrives at is replaced when the client settings are read later on and the whole value is parsed properly. [`DestNet`](/keys/destnet/#scope-client-settings) covers that parse, the format the value is written in, and the buffer it can overrun.
