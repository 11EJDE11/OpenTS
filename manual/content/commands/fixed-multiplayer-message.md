---
command_id: fixed:multiplayer-message
---

A function key opens a message editor only while all of these hold:

- the session is neither a campaign nor a skirmish game;
- the key falls in the session player range, which runs from `F1` to `F1` plus the session's player limit less one;
- no message is already being edited.

Past that gate the session type settles what the key addresses. In IPX and Internet sessions the last key in the range addresses every player at once, and an earlier key addresses one connection — the one at the position the key names, which requires both that a connection stands at that position and that see-all mode is off. Null-modem and modem sessions recognize only two keys out of the range, `F1` and the last, and both open the same editor, labeled as a message rather than addressed to anyone.
