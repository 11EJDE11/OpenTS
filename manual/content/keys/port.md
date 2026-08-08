---
key: Port
summary: The serial port opened for a modem or null-modem game, given as an I/O address.
see_also: ["IRQ", "Baud", "ModemName"]
when_omitted:
  kind: value
  value: "0"
---

`3F8h` opens COM1, `2F8h` COM2, `3E8h` COM3 and `2E8h` COM4. `1` is a special value meaning the port is named by [`ModemName`](/keys/modemname/) rather than addressed. Every other value, `0` included, is refused: the serial menu grays out its dial, answer and null-modem buttons and opens the settings dialog straight away.

Hexadecimal may be written either as `2F8h` or as `$2F8`; a bare number is read as decimal. The trailing-`h` form is the one the game writes back.

:::caution[A broken speed or interrupt discards the port as well]
A baud rate or an interrupt of `0` resets the port, the interrupt and the baud rate together as the settings are read, so a port chosen beside `IRQ=0` never survives to be opened.
:::
