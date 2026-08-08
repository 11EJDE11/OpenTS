---
key: StoreNick
summary: Keeps the Westwood Online nickname and password between runs.
see_also: ["LastNickSlot", "Locale", "Handle"]
when_omitted:
  kind: value
  value: "yes"
---

The value sets the login dialog's remember-me box. Logging in with the box ticked hands the nickname, the password and the location to the chat service to store; logging in without it stores nothing and leaves the logins already kept alone. Either way the state of the box at that moment is written back to `sun.ini`, so the setting follows the last login rather than the file.

The passwords themselves are held by the chat service, not by `sun.ini`.
