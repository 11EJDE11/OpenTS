---
key: EngineerDamage
summary: A figure the rules parser stores for engineer damage that no gameplay path consults.
see_also: ["system:capture", "EngineerCaptureLevel"]
when_omitted:
  kind: value
  value: "0"
---

The value is read and carried in the rules, and it survives a save, but nothing asks for it
afterwards, so writing it changes nothing a player can see. It sits beside
[`EngineerCaptureLevel`](/keys/engineercapturelevel/), which does decide something — the
cursor an engineer shows over a damaged structure — and what an engineer actually does on
arrival is settled from the flags at the structure rather than from either figure.
