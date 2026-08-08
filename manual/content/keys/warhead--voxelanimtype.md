---
key: Warhead
scope: voxelanimtype
label: Voxel debris warhead
see_also: ["Damage", "DamageRadius", "ExpireAnim"]
when_omitted:
  kind: value
  value: none
---

The warhead carries [`Damage`](/keys/damage/#scope-voxelanimtype) at both points a voxel animation deals it: to each object standing within [`DamageRadius`](/keys/damageradius/#scope-voxelanimtype) of a bounce, and as the blast where the piece's life ends. Its armor multipliers and its [`Spread`](/keys/spread/#scope-warheadtype) decide what that damage is worth against a given target, and its [`Bright`](/keys/bright/#scope-warheadtype) flag decides whether the impact throws a flash of light.

A piece with no warhead named deals nothing at either point: the blast is refused before it starts, and the bounce reduction returns zero damage. That zero is written back over the type's stored [`Damage`](/keys/damage/#scope-voxelanimtype) — the same shared figure the [`DamageRadius`](/keys/damageradius/#scope-voxelanimtype) caution covers — so the first bounce that reaches an object leaves that figure at nothing for every piece of the type for the rest of the session. Such a piece still bounces, still plays its effects, and still craters or seeds the ground it lands on.
