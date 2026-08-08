---
key: ShowAll
summary: Games from every Westwood Online lobby listed, not just the player's own.
see_also: ["AllowFind", "LobMusic"]
when_omitted:
  kind: value
  value: "1"
---

With zero, the games list drops every channel whose lobby number differs from the lobby the player joined. With a non-zero value the lobby number is not consulted, and every channel that survives the expansion check — and, in a World Domination Tour session, the territory check — is listed. The lobby's options dialog carries the setting as a checkbox and writes the new state back to `sun.ini`.

The value is loaded as a Westwood Online session starts; no LAN, modem or skirmish path reads it.
