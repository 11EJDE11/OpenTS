# OpenTS

> [!IMPORTANT]
> OpenTS is under active development. Visual Studio 2022 Win32 Debug and
> Release builds are supported for development; runtime behavior is not yet
> validated and there is no supported player release.

OpenTS is an open-source continuation of the reconstructed *Command & Conquer:
Tiberian Sun* engine. It gives equal weight to maintaining a playable engine
and providing a capable platform for modding and engine development.

## Current state

OpenTS is publicly available as an engine-source and development project. The
active version is `0.1.0`. The repository contains no original game assets, and
build support does not establish runtime or player-release support.

The supported target is 32-bit Windows with Visual Studio 2022 in Debug and
Release configurations. See [Building OpenTS](docs/BUILDING.md) for the exact
requirements, commands, outputs, and verification boundary.

## Origins

OpenTS continues the community reconstruction preserved in the
[TibSun archive](https://github.com/OpenTS-Developers/TibSun). The
reconstruction began with the GPL-licensed source released with the
[2020 Command & Conquer Remastered Collection](https://github.com/electronicarts/CnC_Remastered_Collection),
was later rebased onto Electronic Arts' fuller
[Red Alert source](https://github.com/electronicarts/CnC_Red_Alert), and
incorporates applicable shared code from EA's published
[Tiberian Dawn](https://github.com/electronicarts/CnC_Tiberian_Dawn),
[Renegade](https://github.com/electronicarts/CnC_Renegade), and
[Generals and Zero Hour](https://github.com/electronicarts/CnC_Generals_Zero_Hour)
repositories.

Those releases provide related and shared engine code, not the Tiberian
Sun-specific engine in finished form. Missing code and behavior were
reconstructed through reverse engineering and function-level comparison
against the English (US) v2.03 Firestorm executable. No leaked, confidential,
or unofficial Westwood source code was used.

OpenTS is an independent community project and is not affiliated with or
endorsed by Electronic Arts.

## Contributing

Bug reports, proposals, documentation improvements, and focused pull requests
are welcome. Read [CONTRIBUTING.md](CONTRIBUTING.md) before starting a change.
Source conventions are in [STYLE.md](docs/STYLE.md).

## License and acknowledgements

OpenTS is licensed under the GNU General Public License, version 3 or later.
Material derived from Electronic Arts source remains subject to the additional
GPL Section 7 terms in [LICENSE.md](LICENSE.md).

[ACKNOWLEDGEMENTS.md](ACKNOWLEDGEMENTS.md) thanks people and communities whose
work made OpenTS possible.
