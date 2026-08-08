---
key: SkipScore
summary: Whether a won campaign mission passes over the score screen.
see_also: [PostScore, EndOfGame, OneTimeOnly]
when_omitted:
  kind: value
  value: "no"
---

```ini title="map file"
[Basic]
SkipScore=yes
```

The score screen is the presentation that follows a won mission, with its own score track and hall of fame. Setting the key removes that screen and nothing else: [`PostScore`](/keys/postscore/) and [`PreMapSelect`](/keys/premapselect/) still play, and the campaign still advances as it would have. The win movie has already been shown by the time the setting is consulted.

A recorded game being played back skips the score screen regardless.
