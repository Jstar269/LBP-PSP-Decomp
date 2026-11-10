#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Compare installed GCC 4.x PSP compilers with the recorded hashes.
#
#   sh check.sh [BUILD...]        BUILD = 4.0.2h 4.0.2x 4.0.4h 4.0.4x 4.0.4c 4.1.0s (default: those installed)
#
# 1. Binary SHA-256 of every installed program against expected-sha256.txt. The recorded binaries embed the
#    maintainer's configured paths (prefix, assembler, linker; README.md "What the hashes depend on"), so they are
#    reproduced byte for byte only by a build configured with those same path strings; any other prefix gives
#    different driver, cc1, cc1plus and collect2 bytes.
# 2. The path-independent code-generation fingerprint (fingerprint.sh) against fingerprints.txt: this one must
#    match for any prefix.
# Prints CHECK_OK when every fingerprint matches (binary differences are reported, not fatal).
set -eu
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=${LBP_GCC4_ROOT:-$HOME/lbp-gcc4}
if [ $# -eq 0 ]; then
  for b in 4.0.2h 4.0.2x 4.0.4h 4.0.4x 4.0.4c 4.1.0s; do [ -x "$ROOT/psp-gcc-$b/bin/psp-gcc" ] && set -- "$@" "$b"; done
fi
[ $# -gt 0 ] || { echo "no psp-gcc-<build> under $ROOT" >&2; exit 2; }
bad=0
for b; do
  same=0 diff=0
  grep "  psp-gcc-$b/" "$HERE/expected-sha256.txt" > "${TMPDIR:-/tmp}/lbp-gcc4-check.$$"
  while read -r h f; do
    got=$(cd "$ROOT" && sha256sum "$f" 2>/dev/null | cut -c1-64) || got=missing
    if [ "$got" = "$h" ]; then same=$((same + 1)); else diff=$((diff + 1)); echo "BINARY_DIFFERS $f ${got:-missing}"; fi
  done < "${TMPDIR:-/tmp}/lbp-gcc4-check.$$"
  rm -f "${TMPDIR:-/tmp}/lbp-gcc4-check.$$"
  echo "BINARIES psp-gcc-$b identical=$same different=$diff"
  want=$(grep "^FINGERPRINT psp-gcc-$b " "$HERE/fingerprints.txt" | cut -d' ' -f3)
  got=$(sh "$HERE/fingerprint.sh" "$ROOT/psp-gcc-$b" | cut -d' ' -f3)
  if [ -n "$want" ] && [ "$got" = "$want" ]; then echo "FINGERPRINT_OK psp-gcc-$b $got"
  else echo "FINGERPRINT_DIFFERS psp-gcc-$b got $got want ${want:-none}"; bad=1; fi
done
[ $bad -eq 0 ] && echo CHECK_OK || { echo CHECK_FAIL; exit 1; }
