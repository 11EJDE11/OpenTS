---
key: ModemName
summary: The Windows modem opened when the port is named rather than addressed.
see_also: ["Port", "Compression", "ErrorCorrection", "Baud"]
when_omitted:
  kind: value
  value: ""
  note: An empty name, which only matters when the port is given as an address.
---

The name reaches the serial code only while [`Port`](/keys/port/) is `1`, the value that means the port is opened by name instead of by address. The first ten modems Windows lists are then walked in order and the first whose name matches is opened by its own device name; with no match the text is handed to Windows as a device name in its own right. Matching a listed modem is also what makes [`Compression`](/keys/compression/) and [`ErrorCorrection`](/keys/errorcorrection/) mean anything.

The COM settings dialog opens on the matching modem, or on a custom entry carrying the text when nothing matches.

A value equal to the placeholder the read falls back on when the entry is missing — `[NONAME]` in an English build — is cleared back to nothing, so writing it in by hand is the same as leaving the entry out.
