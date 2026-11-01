## What this adds or changes

## Matcher output

<!-- Paste the output of `python tools/lbpmatch.py <every file added or changed>` (status lines only). -->

```text
```

## Checks (see CONTRIBUTING.md, "Checks before you finish")

- [ ] `python tools/lbpmatch.py <every file added or changed>` reports EXACT for each one
- [ ] `python tools/lbpprcheck.py --base origin/main` reports OK
- [ ] Each new source has a true line-2 provenance note, and no other source already covers the function
- [ ] New compiler findings, if any, are added to `docs/TOOLCHAIN.md` ("Findings log")
- [ ] No game data, decrypted modules, disassembly, keys or compiler binaries are included, and nothing from the game
      was posted to a web tool
- [ ] I wrote this change (or name its source above), may submit it under GPL-3.0-or-later, and name any AI tool I
      used

The maintainer re-verifies every source before accepting it and applies accepted changes in the maintainer's tree
(CONTRIBUTING.md, "Pull requests").
