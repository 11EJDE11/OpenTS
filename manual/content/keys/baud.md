---
key: Baud
summary: The line speed the serial port is opened at.
see_also: ["Port", "IRQ", "ModemName"]
when_omitted:
  kind: value
  value: "-1"
---

The value is passed to the port as it is opened, both when the port is probed and when a connection is made over it. `-1` marks the port as unconfigured: the serial menu grays out its dial, answer and null-modem buttons and opens the settings dialog, which rewrites `-1` as `57600`. `0` resets the port, the interrupt and the baud rate together.

A modem that does not answer at the first attempt is retried once at a higher speed, but only from two starting points: `14400` is retried at `19200` and `28800` at `38400`. Any other speed gives up at once. The retried speed replaces the setting, so it is the one written back to `sun.ini`.
