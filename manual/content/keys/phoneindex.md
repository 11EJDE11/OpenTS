---
key: PhoneIndex
summary: The phone book row the modem dialer opens on.
see_also: ["DialMethod", "CallWaitString", "Port"]
when_omitted:
  kind: value
  value: "-1"
---

The number selects a row of the `[PhoneBook]` list, and `-1` means no row is selected. Opening the dialing dialog corrects an unusable number before showing the list: a value below `-1`, or any value at all when the book is empty, becomes `-1`, and a value past the last row becomes `0`.

The row settled on supplies the port settings for the call whenever it carries a port of its own; a row whose port is `0` falls back to the serial defaults instead.
