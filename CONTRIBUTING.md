# Contributing to OpenTS

OpenTS welcomes focused bug reports, proposals, documentation improvements, and
code contributions. Visual Studio 2022 Win32 Debug and Release builds are the
supported development target. A successful build is not runtime evidence, and
every contribution must distinguish the two.

## Before starting

- Search existing issues and pull requests.
- Open an issue before investing in a large feature, intentional behavior
  change, subsystem replacement, or compatibility break.
- Read [Building OpenTS](docs/BUILDING.md) and [Style](docs/STYLE.md) before
  changing source.
- Never commit game assets, original executables, proprietary SDKs,
  credentials, personal data, IDE state, or build output.

## Contribution workflow

Public contributions are pull-request first. Maintainers may commit directly
when appropriate. Pull requests are squash-merged, so keep the proposed change
focused and give it a summary that can become a clear project-history entry.

Separate mechanical cleanup from behavior changes. A formatting pass, rename,
or ownership refactor must not conceal a gameplay, format, persistence, or
network change.

Classify a source change as one of the following:

- **Preserved behavior:** internal implementation changes while relevant
  external behavior remains the same.
- **Bug fix:** OpenTS corrects behavior that is defective for the project's
  goals, whether inherited or newly introduced.
- **Intentional behavior change:** OpenTS chooses a different result and
  documents its compatibility effects.

The TibSun reconstruction and original executable are historical evidence, not
automatic correctness or acceptance criteria for active OpenTS development.

## Compatibility boundaries

Treat documented configuration and modding behavior, game-data formats and
defaults, saves and replays, network messages, deterministic simulation, COM
interfaces, and layout-sensitive structures as external boundaries.

Before changing one of these boundaries:

1. Establish the current behavior and supporting evidence.
2. Identify affected versions, data, mods, saved state, peers, or consumers.
3. Add focused tests or other reproducible evidence.
4. Update the owning documentation in the same change.
5. Provide practical migration guidance for an incompatible change.

Cross-version save, replay, network, and ABI compatibility is not implied
unless a documented contract explicitly provides it.

## Source changes

- Target C++20 for new and substantially rewritten C++ while modernizing
  inherited code incrementally.
- Shape new work so the engine's incremental migration toward an
  entity-component architecture stays possible;
  [Project direction](docs/DIRECTION.md) records the direction.
- Follow surrounding naming and layout; do not format unrelated code.
- Keep honest reconstruction placeholders until evidence supports a better
  name.
- Preserve historical file headers and all SPDX, copyright, modification, and
  GPL Section 7 notices.
- Correct an inaccurate ordinary historical comment narrowly when current code
  or stronger evidence proves it wrong.
- Use `//` or the established block form for ordinary prose. Reserve `///` for
  genuine XML documentation.

Do not remove or consolidate an uncertain file notice. Retain it and establish
the file's history before making a legal or attribution change. The controlling
terms are in [LICENSE.md](LICENSE.md).

## Documentation

Every contribution must account for its documentation impact. Changes to
behavior, interfaces, configuration, commands, scripting, compatibility,
architecture, build procedures, or contributor workflows must update their
owning documentation in the same contribution. A purely mechanical, test-only,
or internal refactor may need no prose change, but the contributor must state
why the existing documentation remains accurate.

Player- or modder-visible engine changes must update the
[OpenTS manual](manual/README.md) and the applicable lifecycle record in the
same contribution.

Give each fact one owner and link to it instead of copying it between guides.
Document current behavior, supported inputs, relevant limitations, and
migration requirements; do not turn plans or assumptions into current-state
claims.

AI tools can help draft, restructure, and review documentation. Contributors
are encouraged to use them when useful, but remain responsible for checking
every claim against current source or observed evidence, applying project
style, and reviewing the final result. AI output is not evidence and must not
receive commit attribution.

For manual content, read [Authoring](manual/AUTHORING.md) and
[Manual style](manual/STYLE.md). Changes to manual tooling, schemas, generated
data contracts, lifecycle machinery, routes, or publication behavior also
require [Maintaining](manual/MAINTAINING.md).

## Validation

Run the narrowest relevant check first. Report exact commands, configurations,
environments, and results, plus material checks that were not run. A configured
project, a successful build, and a runtime observation are different results.

For source changes, build the affected supported configuration. Build both
Debug and Release when changing shared build configuration,
compiler-conditional code, or behavior that may differ under optimization.
Existing MSVC warnings remain; identify new warnings instead of treating the
current warning set as clean.

Behavior changes need focused, reproducible evidence. Automated checks must not
depend on proprietary game assets or original executables.

## Pull request content

A pull request should include:

- a concise summary and rationale;
- the change classification and affected compatibility boundaries;
- exact validation results and material checks not run;
- documentation changes, or why none are needed;
- screenshots or recordings for visual changes when they add useful evidence.

Contributions are submitted under [the repository license](LICENSE.md),
including its applicable additional terms.
