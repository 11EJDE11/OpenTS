---
key: LangFilter
summary: Language filtering asked of the Westwood Online chat service.
see_also: ["AllowPage", "AllowFind", "LobMusic"]
when_omitted:
  kind: value
  value: "1"
---

A non-zero value asks the chat service to filter offensive words out of the messages it delivers; zero asks for them unaltered. The filtering is done by the service, so the request has to be repeated to it each session. That happens once the chat lobby has been reached, and again whenever the lobby's options dialog is accepted.

The value is loaded as a Westwood Online session starts; no LAN, modem or skirmish path reads it.
