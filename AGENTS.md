# Agent guide

Routing and integrity rules for coding agents (and people) working in this repository: a matching decompilation of
the PSP game LittleBigPlanet (UCUS98744 v2.00). The method and most tools follow the Hot Shots Tennis: Get a Grip
decomp; this file says which document to read for which task and lists the rules that are never relaxed.

## Game data never enters this repository

- Never commit or attach retail bytes, decrypted modules, disassembly dumps, decompiler output of whole modules, keys
  or anything else taken from the disc. Every build reads the user's own copy of the game from the gitignored
  `inputs/` and `campaign/`.
- Never post retail bytes, disassembly or decompiler output to a public or web service (decomp.me scratches,
  pastebins, issues, web tools).
- Machine paths live only in the gitignored `lbp-config.json`.
- Sony SDK code the game links (FIOS, SCREAM, SceRT, the SNC library, crt0) is not reconstructed here; `ORIGINS.json`
  gives the origin class of every source.

## Read first

| Task | Read |
| --- | --- |
| Setting up from your own disc, the tools | [README.md](README.md), [docs/GUIDE.md](docs/GUIDE.md) |
| Road to byte-identical 100% | [docs/PLAN.md](docs/PLAN.md), [docs/COMPLETION.md](docs/COMPLETION.md) |
| Matching a function: method, file format, note line | [CONTRIBUTING.md](CONTRIBUTING.md) |
| Compiler behaviour and the compiler builds | [docs/TOOLCHAIN.md](docs/TOOLCHAIN.md), [toolchain/gcc4/README.md](toolchain/gcc4/README.md) |
| Licences | [THIRD_PARTY.md](THIRD_PARTY.md), `REUSE.toml` (`python tools/lbpreuse.py check`) |

## Matching rules

- **Starter functions are for new contributors.** `data/starter-issues.json` lists the functions that an open
  `good first issue` offers; `tools/lbpqueue.py` reserves them, so lane batches never include them, and
  `tools/lbpnearperm.py` never permutes them. Agents never match, permute or install one, not even as a helper; the
  issue's contributor does. To open a starter issue, add its functions to the file in the same change; when the issue
  closes, remove them. `python tools/lbpqueue.py starters` lists the entries that a source now covers.
- **Skills.** Where your tool supports skills, use the lemiur matching skills (`orchestrator`, `matcher`,
  `wall-identifier`, `helper-recovery`, `permute`, `decomp-code-rules`; source:
  <https://github.com/lemiur/Game-Decompilation-Skill-For-Claude>). This file overrides their defaults.
- **Typed struct members, never fixed-offset pointer casts.** Reach a field through a member of a declared struct or
  class, never through a pointer plus a constant offset cast to another type. Every new match must do so, or it is
  not accepted. A source accepted before 2026-11-23 that uses such a cast stays counted in the byte-match figure
  (that figure is never lowered) but is not clean: a separate clean figure, to come with the tools, leaves it out,
  and a standing lane retypes these sources.
- **One home per type.** A type that several sources use is declared in one header under `include/lbp/` and nowhere
  else. To change or extend it, edit the header that owns it; never declare it again in a source or in a second
  header. A cleanup pass merges the duplicates that exist.
- **No fakematches.** Nothing is written only to steer the compiler; the `decomp-code-rules` skill lists what is a
  fakematch and what is a genuine idiom. The one tolerated case is a steering trick under the `MATCH-PENDING` rule
  below.
- **Readability from the first commit.** Names, types and structure are in place when a source is first committed,
  not left for a later clean-up.
- **Debug symbols where they exist.** Before inventing a name or a type, check debugging.games for a build of this
  game or a related title that has symbols.

## Integrity rules (never relaxed)

- **Only EXACT counts.** A source is a match only when `python tools/lbpmatch.py <file>` reports `EXACT` for the
  installed file: the compiled and linked bytes equal retail with no masking.
- **Real source only.** No machine code in a source: no `asm` statements that emit instructions, no
  `.word`/`.byte`/`.incbin`, no code-emitting pragmas, no data steered into `.text`, no `extra=` header or `--flags`
  result in `src/`. `tools/lbpgate.py` refuses such a source (`EXACT_FAILS_GATE`).
- **One exception, inline VFPU asm.** LittleBigPlanet's sources used inline VFPU assembly. Where the original clearly
  used it, a matched source may contain `asm`/`__asm__` statements whose templates hold **only VFPU instructions**,
  provided the header carries `asm=vfpu` and a comment gives the reason on a line starting `Original inline VFPU asm:`.
  Any other instruction in a template, or a missing key or reason, is refused.
- **No steering tricks presented as code.** A duplicated condition, a dummy local or parameter, `volatile`, a
  self-assignment, a gratuitous cast or a `goto` that is there only to steer the compiler is allowed only when it is
  the only exact form found and the note line marks it `MATCH-PENDING matching idiom: <what>`. Such a source counts
  in the byte-match figure, is left out of the clean figure and is queued for a real fix; without the marker it is
  not accepted. A note written before 2026-11-23 that says only `matching idiom:` means the same.
- **Relocatable.** Name every address through a symbol (`FUN_`/`DAT_` address names, or a line appended to
  `symbols/<module>.txt`), never through a numeric constant cast to a pointer.
- **Never edit the verdict.** Do not edit `tools/` as part of a matching change; `symbols/` is append-only.
- **Never lower a figure.** The 100% definition in `docs/COMPLETION.md` is fixed at its strictest.
- **Shared resources.** `--parallel 4` at most for `lbpmatch.py` on a shared machine.
