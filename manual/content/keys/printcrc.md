---
key: PrintCRC
summary: The playback frame at which the game writes a synchronization dump and quits.
see_also: ["Frame", "Type", "CheckHeap"]
when_omitted:
  kind: value
  value: "2147483647"
---

Recording playback compares the frame counter against this number just after computing each frame's checksum. On reaching it the game writes `SYNC<n>.TXT`, where `<n>` is the local player's house number, and then exits. The file carries the build details, the frame and timing counters, the network settings in force and a per-object checksum breakdown, which is what makes two machines' dumps comparable.

:::caution[Arming playback normally skips the read that raises this number]
The sync-bug settings are read as the multiplayer menu is entered, and only while playback is armed. Arming playback with a readable recording present makes the game bypass that menu and start the playback directly, leaving the number at the `0` it was constructed with. The comparison then succeeds on the very first frame, so the dump is written and the game exits before the recording has played anything.
:::
