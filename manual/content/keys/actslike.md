---
key: ActsLike
summary: The country a scenario's house is treated as belonging to when only two sides can be told apart.
see_also: [Owner, GDIHunterSeeker, NodHunterSeeker, "system:production"]
when_omitted:
  kind: computed
  note: An identifier beginning with GDI gives the first country in the rules, one beginning with Nod gives the second, and any other identifier gives the first as well.
---

The value is a country's *position* in the rules country list, written as a number. It is not a country identifier: the read takes a plain integer, so a name written here is read as zero and lands on the first country. Only a campaign mission reads its house records, so this is a campaign setting.

```ini title="scenario map file"
[Special] ; a house record in the scenario's own house list
ActsLike=1 ; behave as the second country in [Houses], Nod in the stock rules
```

Three things consult it, and two of the three can tell apart only the first two countries.

- Every object the house creates copies the value into a record of its own, which is what survives a capture. A construction yard can only put up a BuildingType whose [`Owner`](/keys/owner/) list includes the country its own copy names, so a captured yard keeps building for the country it was built by.
- A hunter-seeker discharge creates [`GDIHunterSeeker`](/keys/gdihunterseeker/) when the value is the first country and [`NodHunterSeeker`](/keys/nodhunterseeker/) for anything else.
- An AI trigger restricted to one side runs only for a house whose value matches: the first country for its GDI restriction, the second for its Nod restriction.

The default is worked out from the country's own identifier before the scenario is read, and it looks at the first three letters only. A country named `GDI-Reserve` therefore starts out as the first country, and a country whose identifier matches neither prefix — which includes the stock `Neutral` and `Special` — is corrected from no country at all to the first one as the house record is read.
