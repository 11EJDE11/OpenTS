# Rationale

This document explains why the reconstructed source looks the way it does:
the tooling the reconstruction rested on, how the original structure was
recovered, and the reasoning behind choices a reader could mistake for
accidents. It grows as questions recur; [History](HISTORY.md) records where
the source came from, and [Project direction](DIRECTION.md) records where
the engine goes from here.

## The tools of the reconstruction

Reconstruction of this kind rests on a small set of tools:

- The original compiler, or the closest estimate of it. For binaries built
  with MSVC 6 or later, the linker records the tools that produced them in
  the Rich header of the PE header, so the right environment can be read out
  of the binary itself.
- An interactive disassembler/decompiler that produces pseudo-C from the
  assembly, preferably one that keeps a database, so accumulated naming and
  types are saved.
- A tool that builds object files from assembly split out of the binary, or
  from the binary directly.
- [objdiff](https://github.com/encounter/objdiff), which compares those
  objects against the objects the reconstructed source compiles to.

## Recovering module file names

Most file names survive in the Red Alert and Renegade releases, and the
games themselves give a great many away: Tiberian Sun through Yuri's Revenge
print a lot of the file names in their debug and error output. A name found
in none of those places is an educated guess, and the guesses can be made
with some precision. The MSVC IDE keeps a project's files sorted alphabetically,
and the MSVC linker links modules in the order the IDE provides, so a
module's position among known neighbors — together with what its code is
for — narrows the plausible names, and sorting alphabetically shows quickly
whether a guess fits well enough.

## Underscore-prefixed file names

`code/` carries dozens of files with a leading underscore — `_map.cpp`,
`_rules.cpp`, `_mixfile.cpp`, and their kin, grouped in the Visual Studio
solution as init files. They hold globals and static initializations.

Analysis of the binary showed a large share of the globals and static
initializations sitting at the very start of the image. In Red Alert most of
them live in `globals.h` and `globals.cpp`; for Tiberian Sun, Westwood
clearly split them up to reduce how much sits in `globals.cpp`. The only
logical way those files end up at the top of the binary is a name that sorts
to the top of the project list, because the IDE sorts its files
alphabetically and [the linker links modules in that order](#recovering-module-file-names)
— and an underscore, sorting before every letter, is the most logical
prefix. The released Red Alert and Renegade sources confirm that this is
what Westwood did: underscore-prefixed files are present in both, from
`_WSPROTO.CPP` beside `WSPROTO.CPP` in Red Alert's `CODE` to the many spread
across Renegade's `wwlib`.

## Definition headers: the `.hh` files

Red Alert defines nearly everything in one singular `defines.h`. The
reconstruction deliberately eliminates that arrangement: each set of
definitions lives in its own `.hh` file, separating definitions from
anything else. A `.hh` file carries definitions only — nothing that performs
static initialization, and no nontrivial inline functions.

The intent is to eliminate the compilation cost of reaching these types
through normal `.h` headers. Including a type through a class header means
the compiler considers — and partly constructs — everything else that header
carries; put an enumeration in a class header and include it across many
files, and the difference in compilation time is significant. A
definition-only header keeps the cost of reaching a type small.

The extension itself is evidenced: rules.ini's comments mention `.hh` files
when referring to definitions and enumerations, which is why the files are
`.hh` rather than `.hpp`.

## Recovering dialog, control, and string names

The names in the language resources come from Red Alert 2. It handles
dialogs much differently, which exposes likely control and dialog ID names,
and both Tiberian Sun and Red Alert 2 debug-print some of them in some game
versions.

The string defines were reconstructed the same way. Red Alert 2 indexes its
strings by label and carries a string table holding those labels, so
cross-referencing it against the Tiberian Sun string table recovers the
original string defines. The only exceptions are the Firestorm additions,
whose strings are not present in the Red Alert 2 string table.

## Assigning code to modules

The original Tiberian Sun code had static initializations sitting in
headers, which polluted nearly every module with duplicate initializations
of the same globals. Those duplicates are a reliable marker: where a new set
of them begins, a new module's functions and data begin, so code could be
assigned to its module quite accurately.

## Why the binary does not match exactly

The reconstruction is not a perfect decompilation that reproduces the exact
2.03 executable, and a completely matching binary was never the goal. It is
also not attainable: producing one would require the DBG symbol file or a
PDB, and without them the information needed is lost and unrecoverable. The
goal was to binary-match every function, the data they reference, and the
loose data spotted while analyzing the binary; it is quite possible that
unreferenced data remains uncaught.

Around 500 functions do not binary-match, and neither do most data sections.
Some of them may never be matchable, though a future Red Alert 2
decompilation may still reveal how to match some of the functions. The
technical reasons are many:

- MSVC of that era has inconsistent behaviors: the accuracy of neighboring
  code affects how other code is generated.
- Function inlining is an unpredictable tax on code generation: one missing
  inline, or one too many, and the function may not match.
- The `static` keyword shapes code generation too. `static const` variables
  are optimized out by the compiler precisely because they are constant, yet
  they still affect the generated code significantly. `Dropship_Screen` is
  the prime example: it went from a 65% match to a 98% match once all the
  correct `static const` values were used. In most cases those constants can
  be found in the binary's data section as unreferenced data.
- An object's BSS section is sorted by a hash of each symbol's name, so
  without the true name of a BSS datum the original order cannot be
  reproduced. The one exception is a known hashing algorithm, with which
  names that sort the same way can be brute-forced or guessed.
- The compiler may swap vtables and alter RTTI entry flags seemingly at
  random.
- Include order affects code generation and data order — most significantly
  the order of vtables and other compiler-generated data, and the order of
  inlines.

Two divergences were chosen deliberately. The duplicated static
initializations described under
[Assigning code to modules](#assigning-code-to-modules) were eliminated,
because they were very confusing to work with during the decompilation. And
includes are sorted by a fixed rule order, which is known to break some
data-order matches in some modules. With a totally matching binary out of
scope, both costs were accepted.
