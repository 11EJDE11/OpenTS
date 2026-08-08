---
key: NetCard
summary: Which network adapter an IPX game is sent over, as a position in the adapter list.
see_also: [Socket, DestNet]
when_omitted:
  kind: value
  value: "0"
---

The figure counts from zero over the bound IPX adapters the machine reports, in the order the transport enumerates them, and it is handed to the transport each time the game socket is opened. The adapter chosen supplies the address the socket is bound to; a figure naming no adapter fails that lookup, the socket is closed again, and the IPX transport does not come up.

The network options screen lists the adapters it finds and stores the position selected, falling back to the first when the stored figure is negative or past the end of that list. Leaving the options screen writes the setting back to `sun.ini`.
