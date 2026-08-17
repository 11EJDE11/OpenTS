# Building OpenTS

> [!IMPORTANT]
> Visual Studio 2022 Win32 Debug and Release builds are supported. Both have
> been verified from a fresh CMake configuration. A successful build
> establishes compilation, not runtime behavior.

## Supported target

| Component | Requirement |
| --- | --- |
| Host and architecture | Windows, 32-bit (`Win32`) target |
| Processor | SSE2, so a Pentium 4 or Athlon 64 onward |
| Generator and compiler | Visual Studio 2022 MSVC 19.30 or newer |
| Windows SDK | A Visual Studio-installed Windows SDK |
| CMake | 3.23 or newer |
| C++ language level | C++20 |
| Configurations | Debug and Release |

Other generators, compilers, architectures, and configurations are not
supported by the current tree.

Install Visual Studio 2022 with the **Desktop development with C++** workload,
a Windows SDK, CMake 3.23 or newer, and Git for Windows.

## Dependencies

The renderer is built on [bgfx](https://github.com/bkaradzic/bgfx), vendored as the
`thirdparty/bgfx.cmake` submodule and pinned to a tested tag. It carries bgfx, bx, and
bimg as submodules of its own, so the checkout must be recursive:

```powershell
git submodule update --init --recursive
```

A fresh clone can do the same in one step with `git clone --recurse-submodules`.
Configuration fails with instructions if the submodule is missing. Updating the
dependency means moving the submodule to a new tag in its own change.

## Configure and build

Run these commands from the repository root in PowerShell:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A Win32
cmake --build build --config Debug
cmake --build build --config Release
```

CMake normally discovers Visual Studio through the Visual Studio Installer. If
the installation is not registered, provide its installation directory and
product version through `CMAKE_GENERATOR_INSTANCE`.

The generated solution exposes only Debug and Release. Successful builds write
the engine executable under `build/bin/<configuration>/` under its runtime
name and copy the runtime files into `TS_RUN_DIR`, which defaults to `Run/`:

| Configuration | Runtime files |
| --- | --- |
| Debug | `GameD.exe`, `GameD.pdb`, `GameD.map`, `Language.dll` |
| Release | `Game.exe`, `Game.pdb`, `Game.map`, `Language.dll` |

`Language.dll` has the same name in both configurations, so the most recently
built configuration replaces the previous copy in `Run/`. Compiler and linker
intermediates remain under the selected build directory.

## Build identity

Each build records the project version and the current commit, branch, commit
date, and whether tracked files were modified, into a generated header that the
debug log reports at startup. The stamp refreshes on every build, so committing
is reflected without reconfiguring, and an unchanged stamp is not rewritten.

Git is not required. A build with no Git available, or from a source archive
with no repository, succeeds and reports the commit as `unknown`.

## Verification boundary

The supported matrix was verified on August 16, 2026 with CMake 4.3.3, Visual
Studio 2022 Community 17.14.37328.6, MSVC 19.44.35228, and Windows SDK
10.0.26100. Fresh Win32 Debug and Release builds completed successfully. The
builds retain inherited MSVC warnings; warnings are not treated as errors, but
contributions should not add new warnings.

Build verification establishes that the supported toolchain compiles and links
the configured targets and produces the listed artifacts. Runtime behavior is
established separately, by play testing, and is outside this build-support
record.

The repository contains no maps, movies, audio, or other original game assets.
Keep legally obtained runtime data local and outside version control. Do not
commit populated run directories, original executables, proprietary SDKs, IDE
state, compiler output, generated CMake projects, or credentials.
