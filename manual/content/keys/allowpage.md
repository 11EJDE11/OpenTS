---
key: AllowPage
summary: Permission for other Westwood Online players to page this account.
see_also: ["AllowFind", "LangFilter", "ShowAll"]
when_omitted:
  kind: value
  value: "1"
---

A non-zero value permits paging and zero refuses it. The setting is handed to the chat service together with [`AllowFind`](/keys/allowfind/) once the chat lobby has been reached, and the lobby's options dialog carries it as a checkbox, writing the new state back to `sun.ini` and reapplying it without a restart.

The value is loaded as a Westwood Online session starts; no LAN or skirmish path reads it.
