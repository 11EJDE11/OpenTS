---
key: DestNet
scope: client-settings
label: Bridged network address
see_also: [Socket, NetCard]
when_omitted:
  kind: value
  value: ""
---

The value is a network number written as bytes in hexadecimal, separated by periods. It is split at the periods and each piece is read as a hexadecimal number; the first four become the network number. Once four or more pieces have been read, the session is marked as bridged and a broadcast address is built from that network number and an all-ones node, which is where the game then sends alongside the local broadcast. Fewer than four pieces leaves the game broadcasting locally only.

```ini title="sun.ini"
[Network]
DestNet=00.00.12.34
```

An empty value, which is also what an absent assignment gives, skips the parse entirely. The address is re-parsed whenever the settings are saved as well as when they are read.

The network options screen offers the address as an eleven-character field and refuses one whose first four pieces are not all single bytes. Nothing checks that when the value comes from the file.

:::danger[More than ten pieces write past the end of the node address]
The pieces after the first four are stored into a six-byte node address, one per piece, and no count is enforced. An address of eleven or more pieces writes past the end of that buffer, which sits on the stack of the routine doing the parsing.
:::
