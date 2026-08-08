---
key: AllowFind
summary: Permission for other Westwood Online players to locate this account.
see_also: ["AllowPage", "LangFilter", "ShowAll"]
when_omitted:
  kind: value
  value: "1"
---

A non-zero value lets another player's find request report where this account is; zero keeps it hidden. The setting reaches the chat service in the same call as [`AllowPage`](/keys/allowpage/) once the chat lobby has been reached, and the lobby's options dialog carries it as a checkbox, writing the new state back to `sun.ini` and reapplying it without a restart.

The value is loaded as a Westwood Online session starts; no LAN, modem or skirmish path reads it.
