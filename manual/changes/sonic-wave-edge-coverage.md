---
title: Draw the sonic ripple to the view's edge
category: fix
release: 0.1.0
targets: []
credit: [ZivDero]
---

A sonic wave lying against the edge of the view now ripples all the way to that edge.
The drawing walk kept a thin margin — three pixels on two sides, two on the others —
that the original code needed because nothing else kept the ripple's replacement samples
inside the view, and that margin showed as an unrippled band across a wave at the
view's edge. With the samples held inside the view where they are taken, the margin
protected nothing and is gone. In the outermost rows a full lift would leave the view,
so those pixels keep their own color and take only the wave's tint, letting the ripple
fade at the edge rather than stop short of it.
