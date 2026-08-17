---
title: Play network games over UDP instead of IPX
category: feature
release: 0.1.0
targets:
- type: key
  id: Socket
  effect: removed
- type: key
  id: NetCard
  effect: removed
- type: key
  id: DestNet
  effect: removed
- type: command
  id: launch:destination-network
  effect: removed
- type: command
  id: launch:socket
  effect: removed
credit: [ZivDero, tomsons26]
---

Network games are played over UDP. Tiberian Sun reached the other machines on a
local network through IPX, which Windows no longer carries, so network play had
stopped working entirely; it now uses the same UDP that internet play always
used. Games are found by broadcasting on every network the machine is attached
to, and each player answering is recorded at the address the answer came from.
Internet, skirmish, and campaign play are unaffected.

Every player must reach the others on one of the networks their machine is
attached to, and all of them play over the same port, 1234. A router between two
players normally stops the broadcast that finds a game, and one machine can host
only one game at a time. The `-DESTNET` option that once reached a network across
a bridge has no counterpart, because it addressed a bridge that only IPX had.

The network options screen is gone, along with the adapter, socket, and
destination network it set. `sun.ini` no longer reads or writes `[Network]
Socket`, `NetCard`, or `DestNet`, and entries left in the file are ignored.

Network messages, save games, and recordings changed shape, so a game, a save, or
a recording is only exchanged between machines running the same build.
