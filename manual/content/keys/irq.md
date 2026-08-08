---
key: IRQ
summary: The interrupt recorded beside the serial port, kept only as a mark that the port is configured.
see_also: ["Port", "Baud"]
when_omitted:
  kind: value
  value: "-1"
---

The Windows serial code the game uses opens ports by device name and throws away the interrupt number it is handed, so the value never reaches any hardware. What it still decides is whether the serial menu is usable: `-1` grays out the dial, answer and null-modem buttons and sends the player to the settings dialog, which rewrites `-1` as `3` when it opens and never shows the field.

`0` is treated as a broken configuration and resets the port, the interrupt and the baud rate together.
