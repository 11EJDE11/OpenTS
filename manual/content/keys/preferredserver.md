---
key: PreferredServer
summary: The Westwood Online server the login dialog opens on.
see_also: ["Locale", "StoreNick", "LastNickSlot"]
when_omitted:
  kind: unchanged
  note: Whatever server the session already remembered, which is none when the game starts.
---

The stored name is compared against each row of the login dialog's server list as that list is built. A row whose text matches becomes the selection and the server the login is aimed at, and the choice is written straight back to `sun.ini`. With no stored name, or none that matches, the list simply opens on its first row.

An empty value is treated as no name at all. A value that is present is never cleared by a later read, so removing the entry does not undo a server chosen earlier in the same run.
