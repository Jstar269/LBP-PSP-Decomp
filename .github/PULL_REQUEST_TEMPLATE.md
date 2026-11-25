## What this adds or changes

## Matcher output

<!-- For added or changed matching sources in `src/`, paste the status lines from `python tools/lbpmatch.py <paths>`.
     If no matching source in `src/` changes, write N/A. -->

```text
```

## Checks (see CONTRIBUTING.md, "Checks before you finish")

- [ ] For added or changed matching sources in `src/`, `python tools/lbpmatch.py <paths>` reports EXACT for each source
- [ ] `python tools/lbpprcheck.py --base origin/main` reports OK
- [ ] `python tools/lbpgate.py --all --wip`, `python tools/lbpreuse.py check` and
      `python -m unittest discover -s tools -p "test_*.py"` pass
- [ ] For a completed good first issue function, this PR removes its entry in `data/starter-issues.json`
      and its obsolete `wip/` files
- [ ] Each new source has a true line-2 provenance note, and no other source already covers the function
- [ ] New compiler findings, if any, are added to `docs/TOOLCHAIN.md` ("Findings log")
- [ ] No game data, decrypted modules, disassembly, keys or compiler binaries are included, and nothing from the game
      was posted to a web tool
- [ ] I wrote this change (or name its source above), may submit it under the repository's licences (CC0-1.0;
      GPL-3.0-or-later for libstdc++ reconstructions and `toolchain/gcc4/`), and name any AI tool I used

The maintainer re-verifies every source before accepting it and applies accepted changes in the maintainer's tree
(CONTRIBUTING.md, "Pull requests").
