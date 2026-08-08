---
key: CallWaitStringIndex
summary: Which call-waiting prefix is dialed ahead of the phone number.
see_also: ["CallWaitString", "PhoneIndex", "DialMethod"]
when_omitted:
  kind: value
  value: "3"
---

The list holds four entries. `0` to `2` are the stock prefixes for disabling call waiting, and `3` means [`CallWaitString`](/keys/callwaitstring/) supplies the text instead. Whichever is chosen is placed in front of the phone book number as the call is dialed.

:::danger[Dialing with a position outside the list copies from an arbitrary address]
Nothing checks the number against the four entries. Dialing with a position below `0` or above `3` reads a text pointer from outside the four the game filled in and copies whatever it addresses into the dial buffer, so the call is placed with unrelated memory in front of the number, or the game stops there.
:::
