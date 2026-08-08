---
key: LobMusic
summary: Music playing while the player waits in the Westwood Online lobby.
see_also: ["LangFilter", "ShowAll"]
when_omitted:
  kind: value
  value: "1"
---

With a non-zero value, reaching the lobby queues a randomly picked score when nothing is playing, and replaces the main menu or map selection score when one of those is. With zero, whatever is playing fades out. The same choice is applied again each time the lobby's options dialog is accepted, so switching it off silences the music immediately rather than at the next screen.

The value is loaded as a Westwood Online session starts and is acted on only while the chat interface is live; no LAN, modem or skirmish path reads it.
