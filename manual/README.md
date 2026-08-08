# OpenTS manual source

`manual/` contains the public OpenTS manual for players, modders, and engine
contributors. It is built with Astro and Starlight, but its source is a
separate documentation product from the repository policy and build guidance
in [CONTRIBUTING.md](../CONTRIBUTING.md), [BUILDING.md](../docs/BUILDING.md),
and [STYLE.md](../docs/STYLE.md).

The manual combines catalogs derived from the current engine with authored
explanations. Generated records establish what the source reads or registers;
authored pages explain behavior, structure, compatibility, and use.

## Governance

- [Authoring](AUTHORING.md) defines page ownership, evidence, lifecycle, and the
  contribution workflow.
- [Style](STYLE.md) defines the public manual's voice, terminology, examples,
  and Markdown vocabulary.
- [Maintaining](MAINTAINING.md) defines generated contracts, releases, routes,
  extraction changes, and publication behavior.
- [Agent instructions](AGENTS.md) add a small set of rules for agents working
  in this directory.

## Directory map

| Path | Purpose |
| --- | --- |
| `content/` | Authored pages and optional overlays |
| `changes/` | Authored engine lifecycle records |
| `data/` | Generated catalogs and maintained registries or adapters |
| `schema/` | JSON Schemas for generated data and authored frontmatter |
| `tools/` | Extraction, validation, scaffolding, and the contributor launcher |
| `site/` | The Astro/Starlight application, tests, and rendered-artifact checks |

Do not commit game assets, original executables, extracted media, credentials,
or build output anywhere in the manual tree.

## Setup

The Python version is pinned in `tools/.python-version`. The Node and npm
versions are pinned in `site/.nvmrc` and `site/package.json`. From the repository
root, inspect the local toolchain first:

```powershell
python manual/tools/manage.py doctor
```

Install the pinned dependencies when required:

```powershell
python -m pip install -r manual/tools/requirements.txt
Set-Location manual/site
npm ci
Set-Location ../..
python manual/tools/manage.py doctor
```

`doctor --verbose` also prints the resolved executable and version-authority
paths.

## Contributor commands

Run every command from the repository root:

```powershell
python manual/tools/manage.py update
python manual/tools/manage.py serve
python manual/tools/manage.py check
python manual/tools/manage.py scaffold --help
```

The launcher has these exact roles:

| Command | Behavior |
| --- | --- |
| `doctor` | Checks the pinned Python, Python packages, Node, npm, and installed site dependencies. It does not build the manual. |
| `update` | Regenerates `data/ini-keys.yaml`, `data/scripting.yaml`, and `data/commands.yaml` as one transaction, reports changes against a Git base, and runs structural and lifecycle validation. It changes tracked generated files when the engine catalog changes. |
| `serve` | Runs `update` and validation, then starts the Astro development server. |
| `check` | Regenerates into a temporary directory to detect drift, validates all contracts and lifecycle rules, runs the Python and site tests, and builds the site once with render, search, and link checks. It does not update the tracked catalogs. |
| `scaffold` | Creates minimal authored content for a supported page or change type and refuses to overwrite an existing file. Scaffolds remain invalid until every `TODO:` is replaced. |

`update`, `serve`, and `check` accept `--base-ref <revision>`. Without it, the
launcher compares against `HEAD`; continuous integration supplies the relevant
pull-request or pre-push revision.

`check` builds with the removed-entity fixtures, so its `dist` carries three
pages that no published build has and is not a publish artifact; run `npm run
build` in `site` for one. The GitHub workflow additionally installs
production-only site dependencies, builds the publish artifact from that clean
dependency set, and verifies that the artifact leaves the fixture pages out.
Therefore `check` is the complete local manual gate, but it is not a
byte-for-byte replay of every CI setup step.

Each step of `check` prints its own wall clock, so a slow gate can be attributed
rather than guessed at.

Use `update` while changing engine-facing documentation, `serve` when visual
review is useful, and `check` before handoff. Continue with
[Authoring](AUTHORING.md) before editing public content.
