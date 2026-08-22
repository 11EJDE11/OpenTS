---
title: Draw the ion blast's warp to the view's edge
category: fix
release: 0.1.0
targets: []
credit: [ZivDero]
---

An ion cannon blast against the bottom of the view now warps the scene all the way to
the view's edge. The drawer kept a seven-row margin above the bottom that the original
code needed because nothing else kept the warp's fetches inside the view, and that
margin showed as an undisturbed band across a blast at the view's edge. With the fetches
held inside the view where they are taken, the margin protected nothing and is gone. In
the lowest rows a fetch from below would leave the view, so those pixels are left alone
and the warp quiets toward the edge.
