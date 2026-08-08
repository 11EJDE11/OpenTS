---
format_id: ini-syntax
title: INI syntax
summary: Defines the section, key, value, and comment syntax accepted by the OpenTS INI parser.
kind: syntax
applies_to:
  - OpenTS INI files
source_files:
  - code/ini.cpp
  - code/ccini.cpp
---

A section begins with `[`, ends at the first `]`, and supplies the section name between them. An assignment uses the first `=` to separate its key from its value. Surrounding whitespace is removed from both parts.

```ini title="example.ini"
[General]
Name=Example ; text after the semicolon is a comment
```

A line beginning with `;` is a comment. A semicolon also starts an inline comment. Lines before the first valid section and assignments with an empty key or value are ignored.

Loading a second file into a database that already holds sections merges the two rather than replacing what is there: a section the database does not carry is added whole, and an assignment repeating a key already present overwrites that key's value.

## Malformed values

A written value is converted by the reader for the kind of value expected, and three of those readers keep the result without testing that anything was converted.

A three-part vector or offset is split at the commas and each part converted to a number in turn, with nothing testing that the part is there. A value carrying fewer than three parts hands the conversion nothing to convert, and the game stops while the rules are being read, before the scenario starts.

A three-part color is scanned into its three channels in one pass, and how many of them the scan filled is never checked. The channels hold no value of their own beforehand, so the ones the scan did not reach keep whatever was last in that storage. Nothing crashes; the color is simply not the one the file asked for. The hue, saturation and value triplet read alongside it clears its three to zero first, which makes this a gap in the color reader rather than a rule of the parser.

A single floating-point number is scanned out of the value and kept whether or not the scan converted anything. It too holds no value of its own beforehand, so a value the scan cannot read as a number leaves the setting carrying whatever was last in that storage rather than falling back to its default. Nothing crashes; the setting simply holds a figure nobody wrote, and not necessarily the same one from one run to the next. A percent sign anywhere in the value divides the number by 100 after it is scanned, so `50%` reads as `0.5`.

An omitted key is safe in all three. A vector or offset falls back to its default when the file writes nothing, a color formats its own default into a full triplet before it is scanned, and a floating-point setting never reaches the scan at all. Only a value the file actually writes reaches these defects.
