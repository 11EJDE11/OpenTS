# Project direction

The [README](../README.md) states the goals development moves toward; this
document records the architectural direction behind them.

## Toward an entity-component architecture

The inherited object model concentrates state and behavior in one deep class
hierarchy, which is what makes so much of the original game hard-coded. The
project's long-term direction is to modernize the engine incrementally toward
an entity-component architecture, because that is the shape that supports the
de-hardcoding, extensibility, and feature goals on the roadmap. There is no
big-bang rewrite: the engine stays playable throughout, and new development
is shaped so that the migration stays possible — prefer composition over
deepening the inherited hierarchy, keep new state separable from behavior,
and avoid new hard couplings to the class tree.
