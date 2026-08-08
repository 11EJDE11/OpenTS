---
key: Socket
scope: client-settings-2
label: Socket opened at startup
see_also: [NetCard, DestNet]
when_omitted:
  kind: value
  value: "0"
---

This is the earlier of the two reads of the assignment, made before the display is prepared. It falls back on `0` rather than on the stored socket, and only a figure above zero is acted upon: the figure is raised by `16384`, and the result is put into service when it lands below `32768`. A figure of `16384` or more therefore does nothing here, and so does a negative one.

The figure is read as a plain whole number at this point, without the sixteen bit truncation the later read applies, so a figure far outside the sixteen bit range is simply discarded rather than wrapped.

The same assignment is read again with the client settings later on, and that read opens the same socket for every figure this one acted upon. [`Socket`](/keys/socket/#scope-client-settings) covers it, and it is the read the network options screen writes for.
