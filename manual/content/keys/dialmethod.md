---
key: DialMethod
summary: The tone or pulse dialing the modem uses to place a call.
see_also: ["PhoneIndex", "CallWaitString", "CallWaitStringIndex"]
when_omitted:
  kind: value
  value: "T"
---

`T` dials in tone and `P` in pulse. Only the first character of the value is kept, so `Tone` and `Pulse` are accepted just as well, and a first character that is neither falls back to tone.

Each phone book row carries its own copy of the same choice, and a call placed through a row that has its own port settings uses that copy rather than this one.
