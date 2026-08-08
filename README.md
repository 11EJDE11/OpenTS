# Command & Conquer: Tiberian Sun — reconstructed source

This repository contains a community reconstruction of the source code for
*Command & Conquer: Tiberian Sun*. It was produced by the
[OpenTS project](https://github.com/OpenTS-Developers/OpenTS) through reverse
engineering of the shipped game, with portions derived from the Red Alert source
released by [Electronic Arts](https://github.com/electronicarts/CnC_Red_Alert).

This is not an original Westwood Studios or Electronic Arts source release, and
OpenTS is not affiliated with or endorsed by Electronic Arts.

## Preservation status

This is a frozen source snapshot. It is published for preservation and as the
upstream baseline of OpenTS; it does not accept issues, pull requests, or support
requests. Active development takes place in
[OpenTS](https://github.com/OpenTS-Developers/OpenTS).

The reconstruction targets the latest English (US) release:

```text
GAME.EXE; v2.03[EN]; Monday 5th June, 2000 (21:26:42)
MD5: C2C58CBBF83AF0458DC44EF64A3C011F
```

The reconstructed game builds and runs. At the time of the source drop, about
95% of its functions matched the reference executable byte for byte. That figure
records reconstruction progress; it is not a claim that the source is original
Westwood code or that byte equality alone proves correctness.

## Contents

All reconstructed source is under [`code/`](code/), including the reconstructed
Westwood VQA player in [`code/vqalib`](code/vqalib). Obsolete Visual C++ project
and workspace files have been removed; this snapshot contains no supported
legacy build path.

This archive is intentionally not a supported build distribution. It omits the
build definitions, development tooling, and game data. For the maintained Visual
Studio 2022 and CMake build, use the active OpenTS repository.

No game assets are included. A legitimate copy of *Tiberian Sun* is required to
run a compiled game.

## Acknowledgments

The reconstruction was created by CCHyper, tomsons26, ZivDero, and the OpenTS
contributors, with contributions and support from Neijwiert, DarthJane, jonwil,
OmniBlade, Rampastring, CnCNet, ModHaven, and Project Perfect Mod.

## License

The reconstructed source is licensed under the GNU General Public License,
version 3 or later.

Some files contain material derived from Electronic Arts source code released
under that license with additional terms under GPL Section 7. Those files carry
an Electronic Arts copyright notice. See [LICENSE.md](LICENSE.md) for the full
license and additional terms.
