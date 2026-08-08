---
key: ErrorCorrection
summary: The modem's own error correction, switched on or off before dialing.
see_also: ["Compression", "ModemName", "Port"]
when_omitted:
  kind: value
  value: "0"
---

The setting reaches the modem only while [`Port`](/keys/port/) is `1` and [`ModemName`](/keys/modemname/) matched a modem Windows knows about. The command that modem's own entry gives for enabling error correction is then sent for a non-zero value, and its disabling command for zero, as part of the sequence that precedes dialing or answering. A modem Windows does not describe is left however it was.

The COM settings dialog carries the same switch as a checkbox.
