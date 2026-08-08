---
key: CallWaitString
summary: The call-waiting prefix dialed ahead of the number when the custom position is chosen.
see_also: ["CallWaitStringIndex", "PhoneIndex", "DialMethod"]
when_omitted:
  kind: value
  value: ""
  note: An empty prefix, so the number is dialed on its own.
---

The text is used only while [`CallWaitStringIndex`](/keys/callwaitstringindex/) is `3`, the position that stands for a custom prefix; the other three positions take their text from the stock list instead. It is placed in front of the phone book number as the call is dialed. Fifteen characters are kept.
