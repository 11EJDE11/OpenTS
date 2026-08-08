---
key: PlayerLeft
summary: Sound played in a network lobby when a player drops out of the game being set up.
see_also: [PlayerJoined, GameForming, GameClosed, OptionsChanged]
when_omitted:
  kind: value
  value: none
---

```ini title="rules.ini"
[AudioVisual]
PlayerLeft=PLYRLEFT ; a sound ID registered in SOUND.INI
```

The sound belongs to the LAN and Internet lobbies and is played without a position. In the LAN lobby it marks a sign-off arriving for somebody in the player list, and only once the local player's own join has been confirmed; a sign-off from someone who was in the chat list rather than the game is silent. It plays once per matching entry removed.

In the Westwood Online lobby it marks any user leaving the current channel while that channel is not a plain chat channel — including the local player's own departure, which is announced as the channel is torn down.
