---
key: LastNickSlot
summary: The stored login slot a new Westwood Online login overwrites once every slot is full.
see_also: ["StoreNick", "Locale"]
when_omitted:
  kind: value
  value: "-1"
---

The chat service keeps thirty-two login slots. A login asked to be remembered reuses the slot that already holds the same nickname, or failing that the first empty slot, and neither case consults this number. Only when all thirty-two are occupied does the number matter: the login is written into the slot it names and the number then advances to the next one, wrapping back to `1` past the thirty-second, so the slots are recycled in turn.

`-1` starts that rotation at the first slot. `0` names no slot, and a login that falls back on it is silently not stored.
