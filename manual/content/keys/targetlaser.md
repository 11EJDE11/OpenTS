---
key: TargetLaser
summary: Draws a dashed sighting line from a firing vehicle to where its shot is aimed.
see_also: [Primary]
when_omitted:
  kind: value
  value: "no"
---

```ini title="rules.ini"
[MYSNIPERTANK] ; a UnitType registered in [VehicleTypes]
TargetLaser=yes
```

Every shot the object fires starts a fifteen-frame timer — one second — and while that timer runs the object draws a red dashed line from its turret to the point its shot is aimed at, with a small red marker at each end. The line follows the aim, not the projectile, so it tracks a moving target for as long as the object keeps hold of it and disappears the moment the object loses its target.

Two conditions narrow it well below what the key's presence on every object type suggests. The timer is started only for a house the local player controls, so an enemy's sighting line is never drawn; and only a vehicle draws the line at all, so setting the key on an aircraft, a structure or an infantryman starts a timer that nothing reads.
