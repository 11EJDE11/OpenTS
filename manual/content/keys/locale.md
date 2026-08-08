---
key: Locale
summary: The Westwood Online location the player chats from.
see_also: ["PreferredServer", "StoreNick", "LastNickSlot"]
when_omitted:
  kind: value
  value: "0"
---

The number is a location code the chat service defines, and `0` stands for no location chosen. The login dialog preselects the matching entry in its location box and warns before proceeding while the value is still `0`; a completed login reports the location to the service, and the World Domination Tour's location picker opens on it. The find-games and ladder dialogs open their own location filters on the all-locations entry regardless.

The value read when the multiplayer menu is entered is kept aside as a fallback. Whenever the location has fallen back to `0` at the moment the settings are written out, that earlier value is written in its place, so a location the player once chose is not lost by a session that never reached the service.
