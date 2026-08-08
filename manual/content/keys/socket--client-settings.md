---
key: Socket
scope: client-settings
label: Stored socket
see_also: [NetCard, DestNet]
when_omitted:
  kind: value
  value: "-1"
  note: The figure is written as -1 into an unsigned sixteen bit field, so what the field actually holds is 65535, the figure that selects the unconfigured socket.
---

The figure is kept in an unsigned sixteen bit field, so `-1` is stored as `65535` and any figure outside `0` to `65535` wraps into that span before anything looks at it. What happens next depends on where it lands.

| Stored figure | Socket put into service |
| --- | --- |
| `65535` | `34835`, the number the game uses when nothing was configured |
| Below `16384` | The figure raised by `16384` |
| `16384` through `65534` | None — whatever socket is already open stays open |

The raise keeps a configured socket inside the dynamic range that the transport is willing to bind, so the figure written into the file is not the number that ends up on the wire.

The network options screen offers the figure as a five-digit field, rejects anything above `16383`, and stores an empty field as `-1`. Leaving the options screen writes the setting back to `sun.ini` and puts the resulting socket into service straight away.
