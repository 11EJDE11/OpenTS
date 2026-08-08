# Manual style

Public manual prose should be concise, literal, and useful to a reader making a
configuration or maintenance decision. Structured catalogs and frontmatter
render the reference facts; prose explains behavior and non-obvious context.

## Audience and voice

Modder-facing pages may assume familiarity with Tiberian Sun INI syntax,
ObjectType IDs, and FinalSun terminology. Define OpenTS-specific behavior and
invariants. Developer-facing Internals pages may assume C++ and repository
familiarity, but still define OpenTS-specific ownership and compatibility
boundaries.

Use the vocabulary familiar to the Tiberian Sun modding community, followed by
the exact engine identifier where it matters. Preserve the spelling and case of
keys, IDs, enum tokens, filenames, command strings, and C++ symbols. Use
American English for authored prose and sentence case for headings and labels.

A C++ class name is not a term of art for readers. Write vehicle, infantry,
aircraft and structure rather than `UnitClass`, `InfantryClass`,
`AircraftClass` and `BuildingClass`, and prefer a plain description to an
engine word wherever an honest one exists. Link the
[glossary](site/src/content/docs/glossary.md) only for a concept that recurs
across many pages and has no short synonym, and never as a licence to leave a
sentence opaque; anything below that bar takes a plain word, or a definition at
its first use on the page that needs it. Where the plain word would be ambiguous,
name the kinds of object meant rather than retreating to the class that groups
them: "unit" alone is ambiguous in this game's vocabulary and cannot carry that
weight.

Write in the present tense with declarative sentences. Lead with the result,
then the condition. Avoid marketing language, personification, editorial
status, and vague claims such as "safe," "normal," "available," or "works" when
the exact predicate can be stated.

Public copy describes the engine and its inputs. It does not call material
generated, extracted, authored, or contract-backed, and it does not narrate the
documentation production process.

The single exception is one site-level notice recording that the manual was
produced with AI assistance and may contain imprecisions. It states the
limitation plainly and briefly, lives in exactly one place, and is never
repeated onto individual pages. A per-page disclaimer would say nothing true
about that particular page and would make verified work look presumptively
unreliable.

## Canonical terms

| Term | Meaning |
| --- | --- |
| Section | A bracketed INI heading such as `[GAPOWR]` |
| Key | The name to the left of `=` |
| Value | The text to the right of `=` |
| Assignment | The complete `Key=Value` line |
| ObjectType ID | A rules identifier that names a type definition |
| Runtime instance | An object created from a type and present in the scenario |
| Image ID | An art identifier selected by an assignment such as `Image=` |
| TypeClass | A C++ type-definition class; use only in developer-facing prose |

Distinguish type-definition state, runtime-instance state, and global or
scenario state. Link the first meaningful use of an engine-specific concept to
its canonical explanation when that link helps the reader.

### Words that carry more than one meaning

Each word below means several things in this game, so the manual fixes each one
to a single sense. None of them is a global substitution: what to write depends
on what the source establishes at the site in hand, so scope every site from its
own code path rather than from a neighboring page.

**Mover and foot object.** Name the kinds of object instead. The model is
[`CloseEnough`](content/keys/closeenough.md), which spells out what a class name
would have hidden: "infantry, walkers, hovercraft and driven vehicles all apply
it." A single phrase can cover different kinds on different pages, so read the
kinds off the code path in front of you.

**Taken off the map.** "Leaves the map" describes an object literally exiting
across the map edge, and nothing else. For an object destroyed, sold, or
otherwise removed from play, write "taken off the map", as
[Laser fences](content/systems/laser-fences.md) does: "Sold, undeployed or
otherwise taken off the map."

**Full strength.** Reserved for a team's roster count — the number of members
the TaskForce asks for, as
[AI triggers and team production](content/systems/ai-team-production.md)
defines it. The hit-point sense is "undamaged" or "maximum strength". The
lighting sense is "full brightness".

**Slot.** Never bare. Qualify it at its first use on the page: difficulty slot,
weapon slot, cameo slot, upgrade slot, Tiberium slot, landing slot.

**Score.** Points. The music sense is "music" or "music track".

**Under way.** Reserved for a team that has started. The motion sense is
"moving".

**Converter and drawer.** C++ names rather than reader vocabulary, and they
stand for three unrelated things: a cell's tint table, the terrain tile
renderer, and the shape renderer. Say what the site means — a cell's tint table
is "the same tinted terrain palette as the ground beneath it" — and settle each
site on its own rather than substituting one phrase for another.

### Map regions

A scenario declares two regions of cells, and they are not the same region. Use
these two names for them and no others.

| Name | Declared by | What it is |
| --- | --- | --- |
| The playfield | `[Map] Size=` | Every cell the map has. There are no cells outside it, so nothing can stand, path, or be revealed there |
| The playable area | `[Map] LocalSize=` | The smaller region inside the playfield that a player can see, scroll to and play in. The engine clips it into the playfield and insets it further, and the ring left over is the map's border |

Both are diamonds on the cell grid rather than rectangles, however their
assignments are written, so do not call either one a rectangle. "The map
rectangle" names a third thing — the upright square of cells that encloses the
playfield, used for scanning and iteration — and needs defining wherever it is
used.

Do not write "the playable diamond", "the visible area", "the local map area",
"the local radar area" or "the scenario's declared playable area". "Playable"
belongs to the smaller region, "visible" collides with what shroud and fog
cover, and the rest are engine wording rather than reader vocabulary.

Do not convert any of them by find and replace. "The playable area" is already
in use for both regions, so an existing use of it is evidence of nothing, and
neighboring behaviors split across the two: crates are placed inside the
playable area and picked up anywhere in the playfield, an object decides whether
to look at all from the playable area and then scans the playfield, and a
guarding unit survives out to the playfield while everything else is deleted at
the playable area's edge. Settle each sentence against its own code path.

## Summaries and structure

Write `summary` as one short, literal sentence describing the page's behavior
or purpose. It is rendered as the subtitle and search description, so do not
repeat it as the opening paragraph or turn it into a teaser.

State the value or the behavior directly rather than framing it. "Names the
WeaponType in the first weapon slot" and "Marks a map as one that shipped with
the game" describe the assignment; "The WeaponType in the object type's first
weapon slot" describes the thing the reader came for. A summary that opens on
the setting's effect also survives a scope being added later, where one that
opens on the act of naming usually does not.

Choose headings by structure rather than appearance. Use H2 for a parent topic
and H3 for its phases, cases, or variants. A short page may need no headings.
Do not expand a straightforward entity to fill a template.

A system page that rests on an entity carrying its own vocabulary — a team, an
AI trigger, a particle system — introduces that entity before it uses it. Give
the reader what the entity is, the few attributes the mechanic actually turns
on, and one short example of how it is written. Keep it to one opening section,
headed so that a reader who already knows can see what to skip. This is not a
licence to explain every noun: a page introduces the entities it is about, not
the vocabulary of the whole game. But a mechanic explained in terms the reader
has to reconstruct from elsewhere is not explained, and the cost of that lands
on exactly the readers who came to learn it.

Define a term of art at its first use on the page, in the sentence that uses
it, however obvious it looks from inside the source. Full strength, autocreate,
a stray distance and a team's origin all read as ordinary English and none of
them mean what they appear to.

When describing a runtime decision, include the order, acceptance condition,
rejected paths, fallback, and result that materially affect use. Name failure
effects precisely when the source establishes them. Do not reduce a known
crash, deletion, desynchronization, collateral effect, or cleanup path to a
generic statement that the operation fails.

Set a condition out as a structured list once it carries three or more terms or
any nesting. Head each group with "All of", "Any of" or "None of" and indent its
terms beneath, so the shape of the test is visible before any of it is read, and
follow the order the engine tests in so a reader can match the list against the
behavior in front of them. A sentence chaining several ands, ors and exceptions
makes the reader reconstruct the branch by hand and hides which term decided the
outcome.

## Examples

Use the smallest concrete input that clarifies a non-obvious syntax, scope,
interaction, unit, or outcome.

- Give each INI fence a useful file title.
- Use exact keys and realistic values rather than pseudo-syntax where possible.
- Define synthetic IDs and every referenced synthetic ID, preferably with a
  short inline comment.
- Identify a fragment when required surrounding configuration is intentionally
  omitted.
- Explain only the fields and result that are not already clear from the
  structured reference.
- Call a value a default, recommendation, or canonical choice only when the
  evidence establishes that claim.

```ini title="rules.ini"
[GAPOWR]
BaseNormal=no
Adjacent=5
```

Examples demonstrate input shape; they are not evidence that a runtime result
was observed. Do not require proprietary assets or original executables to
understand an example.

## Markdown vocabulary

Prefer ordinary Markdown:

- inline code for keys, values, IDs, filenames, sections, and expressions;
- titled fenced blocks for INI, serialized records, and necessary source;
- short lists and tables for real steps, choices, or mappings;
- Starlight asides only when information deserves interruption.

Use `note` for necessary context, `tip` for a source-supported or tested recipe,
`caution` for an easy misconfiguration or surprising exception, and `danger`
for a demonstrated crash, corruption, desynchronization, or compatibility risk.
A danger aside states both the triggering input and the outcome.

Avoid raw HTML, MDX-only widgets, one-off components, empty paragraphs, and
callouts used only to adjust visual hierarchy. Shared Astro components and CSS
own recurring presentation.

## Keep one source of truth

Do not repeat facts already rendered from structured metadata: omission and
no-effect records, generated command fields, scripting value lists, format
fields, accepted-key selectors, source-file lists, or typed relationships.
Prose may explain why a fact matters, but it should not maintain a parallel
copy of the same list.

State current behavior without routine verification hedges or compatibility
promises. When evidence is incomplete, narrow or omit the claim; add a local
qualification only when the uncertainty changes the reader's decision.
