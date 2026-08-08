---
key: InitStringIndex
summary: Parsed modem initialization choice that is overwritten before anything can use it.
no_effect: true
see_also: ["DialMethod", "Port", "ModemName"]
when_omitted:
  kind: value
  value: "-1"
---

The name promises a row of the `[InitStrings]` list to send to the modem ahead of dialing. Phone book rows carry their own copy of the same choice, and that copy is honored; this one is not.

The value is stored, and then, a few lines further into the same read, replaced without any condition attached: the first row when the list has entries at all, and `-1` when it has none. No dialing path ever sees the value that was read.
