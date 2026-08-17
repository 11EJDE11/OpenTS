---
key: Handle
summary: The player name remembered between runs and offered back by the multiplayer dialogs.
see_also: ["Color", "Side", "StoreNick"]
when_omitted:
  kind: value
  value: "[NONAME]"
  note: The English placeholder text; a localized build supplies its own.
---

The name is read as the multiplayer menu is entered and fills the name field of the LAN and skirmish dialogs, where editing the field writes the new text straight back. On the LAN it is also the name the game is advertised under when the player hosts. Nineteen characters are kept, and the LAN dialog limits typing to sixteen.

The Westwood Online lobby does not offer the name back. Logging in replaces it with the nickname the login was made under, and the login dialog's only use for the old one is to preselect the matching entry in the stored nickname list.
