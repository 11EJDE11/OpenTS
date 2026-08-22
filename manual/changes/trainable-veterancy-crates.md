---
title: Honor Trainable for veterancy crates
category: fix
release: 0.1.0
targets:
- type: system
  id: veterancy
  effect: changed
- type: key
  id: Trainable
  effect: changed
credit: [ZivDero, ts-patches contributors, CCHyper]
---

A veterancy crate now promotes only objects whose types have `Trainable=yes`. Its radius, number of promotion steps and lack of an ownership filter are unchanged.
