---
title: Game data
summary: A local developer build reads legitimately owned Tiberian Sun data from the repository's Run directory.
category: getting-started
source_files:
  - README.md
  - docs/BUILDING.md
  - Run/place_steam_build_here
related:
  - type: using
    id: build-and-run
  - type: using
    id: configuration-files
---

The repository contains engine source and build inputs. It does not contain maps, movies, audio, or other proprietary game assets.

Place data from a legitimate copy of Tiberian Sun under `Run/`. The tracked `Run/place_steam_build_here` marker identifies this local run tree; the directory's populated contents are ignored by Git.

Do not place game data in the CMake build directory. The build copies OpenTS executables and `Language.dll` into `Run/`, alongside the locally supplied game files.
