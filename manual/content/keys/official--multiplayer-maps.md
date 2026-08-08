---
key: Official
scope: multiplayer-maps
label: Multiplayer map provenance
see_also: ["Name"]
when_omitted:
  kind: value
  value: "no"
---

Loose `.mpr` files sitting in the game directory are scanned whenever the multiplayer map list is rebuilt, and this flag travels with the map into the lobby, where the host sends it to the guests along with the rest of the game options. What it decides is whether a guest who does not already hold the map may fetch it from the host.

The answer differs by path. A guest missing a map marked `yes` will not ask for it: on a LAN it reports that it cannot play and signs off, and over a modem it tells the host to choose another map. A map left unmarked is transferred from the host instead. In a Westwood Online game the guest asks the host for the file whichever way the flag reads, so the mark changes nothing there.

The generated random map is exempt from the refusal whatever the flag says, and maps packaged inside a `.pkt` list count as official without consulting any setting.
