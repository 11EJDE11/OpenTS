---
key: Facings
summary: The number of facing blocks a shape-drawn vehicle's artwork is cut into.
see_also: ["WalkFrames", "StandingFrames", "FiringFrames", "RotCount", "Voxel"]
when_omitted:
  kind: computed
  note: 8, or 1 for a vehicle that declares no firing frames and no turret.
---

Only a vehicle drawn from shape artwork reads this figure; a [`Voxel=yes`](/keys/voxel/) vehicle is turned rather than picked out of a set of frames.

The setting does two separate jobs. It decides whether the vehicle is drawn per facing at all, and it is the multiplier the engine uses when it works out where the standing, firing and death blocks begin.

Per-facing drawing happens at exactly `8` and at no other value. At `8` the block to draw from is the vehicle's own heading rounded to eight compass points and then advanced by one: a vehicle pointing northwest is drawn from block 0, one pointing north from block 1, and so on round to west at block 7. At any other value the block index is fixed at 0, so every instance is drawn from the first block whichever way it points.

```ini title="art.ini"
[JUGGER] ; the Image ID of the stock Juggernaut
Voxel=no
WalkFrames=15  ; blocks of 15 frames, one per facing, from frame 0
StandingFrames=0
Facings=8
```

The same figure is the stride in every frame number the engine derives rather than reads: [`StartStandFrame`](/keys/startstandframe/), [`StartFiringFrame`](/keys/startfiringframe/) and [`StartDeathFrame`](/keys/startdeathframe/) each multiply a per-facing frame count by it. Raising it therefore pushes those defaults further up the file even though it adds no facings to the drawing, and lowering it to `1` collapses them onto the walk block.

:::caution[Only 8 selects per-facing drawing]
`Facings=32` does not cut a vehicle into 32 facings. It leaves every instance drawn from block 0 while multiplying the derived standing, firing and death start frames by 32, which lands them well past the end of ordinary artwork.
:::
