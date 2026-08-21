---
title: Stop dropping orders once the command queue is full
category: fix
release: 0.1.0
targets: []
credit:
- ZivDero
---

Every vehicle and infantry unit in a selection now carries out the order it is given. Orders
wait in a queue until the frame that sends them, and that queue held sixty-four of them, so
ordering a larger group about discarded the orders past the sixty-fourth without saying so
and left the rest of the group standing where it was. The queue now grows to hold as many
orders as are given at once.

A network or internet game no longer ends with a data queue overflow. Orders arriving from
the other players wait in a second list until the frame they belong to comes around, that
list held four thousand and ninety-six of them, and a player busy enough to fill it was
dropped out of the game with that message.
