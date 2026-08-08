---
key: Win
summary: The movie played when a campaign mission is won.
see_also: [Lose, SkipScore, PostScore, Intro]
when_omitted:
  kind: value
  value: "<none>"
---

```ini title="map file"
[Basic]
Win=UNSTPBLE
Lose=KILLMECH
```

The movie is the first thing shown once the mission has been won, before the score screen and everything that follows it. Multiplayer and skirmish games end their own way and never reach it.

[`Intro`](/keys/intro/) covers how a movie name is resolved and what happens to one that cannot be found.
