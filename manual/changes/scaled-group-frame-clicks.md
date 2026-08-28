---
title: Deliver clicks through group frames when the picture is scaled
category: fix
release: 0.2.0
targets: []
credit:
- ZivDero
---

A control standing inside a dialog's group frame — the thin box drawn around a set of
related options — now takes clicks while the picture is scaled into the window. With
scaling active the game delivers every mouse message itself, to the control standing where
the player sees it, and that delivery handed each click to the topmost window over the
position. The system asks a window whether it lets clicks through, and a group frame does;
the game's own delivery never asked, so a frame drawn over a control kept the clicks meant
for it.

In the skirmish lobby this left the Game Speed slider and the Re-Deployable MCV, Short
Game and Multi Engineer checkboxes ignoring the mouse whenever the window was scaled,
while the controls around them kept working. At the window's native size the game leaves
delivery to the system, and every control responded.
