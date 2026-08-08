---
key: NextParticleOffset
summary: How far a successor particle is placed from the one it replaces.
see_also: ["NextParticle", "Radius"]
when_omitted:
  kind: value
  value: 0,0,0
---

```ini title="rules.ini"
[MYGASSEED] ; a ParticleType registered in [Particles]
Image=gaslrgmk
BehavesLike=Gas
MaxEC=448
NextParticle=MYGASCLOUD ; a ParticleType registered in [Particles]
NextParticleOffset=0,0,150 ; the cloud forms 150 leptons above where the seed died
```

Three lepton components — X, Y and Z — added to the position an expiring particle held when its [`NextParticle`](/keys/nextparticle/) successor is created there. Only gas, weak gas and web systems apply it. A smoke system scatters its pair of successors with [`Radius`](/keys/radius/) and never reads the offset, and fire, spark and railgun systems create no successors at all.

The offset is applied once, at the moment of the replacement, and each link of a chain adds its own: three types that each raise their successor by 150 leptons leave the last of them 300 leptons above where the chain began.

:::danger[A value with fewer than three components crashes the game]
`NextParticleOffset=0,0` stops the game while the rules are being read, before the scenario starts; [INI syntax](/formats/ini-syntax/#malformed-values) explains the incomplete-value defect behind it, and why leaving the key out is safe.
:::
