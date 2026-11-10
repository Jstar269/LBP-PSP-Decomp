#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Build the open-source GCC 4.x PSP cross compilers that lbp-decomp's sources use, from the GNU release tarballs, the
# pspdev/ps2dev Allegrex patches and this project's changes in patches/. See README.md.
#
#   sh build.sh BUILD...        BUILD = 4.0.2h 4.0.2x 4.0.4h 4.0.4x 4.0.4c 4.1.0s, or all
#
# Each BUILD installs the compiler proper (driver, cc1, cc1plus, collect2, gcov) into $LBP_GCC4_ROOT/psp-gcc-BUILD.
# An x build is its h build plus this project's changes: it copies the h work tree, applies the changes, rebuilds
# cc1/cc1plus there and installs a copy of the h install with the new cc1/cc1plus (so it shares the h driver and its
# configured prefix, exactly as the recorded builds did); the h build is made first when its work tree is missing.
#
# Settings (environment; every path must be absolute):
#   LBP_GCC4_ROOT   install root, the prefix you name (default $HOME/lbp-gcc4); configured into the binaries
#   LBP_GCC4_DL     downloaded sources (default $LBP_GCC4_ROOT/dl; see fetch.sh)
#   LBP_GCC4_WORK   work trees and logs, about 0.6 GB per build (default $LBP_GCC4_ROOT/work)
#   LBP_GCC4_AS     assembler the compilers are configured with (default $LBP_GCC4_ROOT/sony-as; when that default
#                   is missing, the sony-as wrapper next to this script is installed there): the Sony binutils 2.14
#                   assembler of the decomp.me psp-gcc-1.3.1 kit, through the wrapper; configured into the driver
#   PSPDEV_BIN      REQUIRED: bin/ directory of a pspdev toolchain (psp-ld, psp-as, psp-objdump, psp-objcopy);
#                   psp-ld is the configured linker, all four are linked into each install's bin/
#   LBP_GCC4_HOST_TOOLS  directory of the host nm, ar, ranlib, strip linked in as psp-nm etc. (default /usr/bin)
#   LBP_GCC4_PATH   PATH for the host build (default /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin)
#   JOBS            parallel make jobs (default 2)
set -eu
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=${LBP_GCC4_ROOT:-$HOME/lbp-gcc4}
DL=${LBP_GCC4_DL:-$ROOT/dl}
WORK=${LBP_GCC4_WORK:-$ROOT/work}
SONY_AS=${LBP_GCC4_AS:-$ROOT/sony-as}
PSPDEV_BIN=${PSPDEV_BIN:-}
HOST_TOOLS=${LBP_GCC4_HOST_TOOLS:-/usr/bin}
HOST_PATH=${LBP_GCC4_PATH:-/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin}
JOBS=${JOBS:-2}
SONY_AS_VERSION="2.14+allegrex-vfpu-2.2.1-psp-1.3.1"

die() { echo "BUILD_FAIL $*" >&2; exit 1; }
for v in "$ROOT" "$DL" "$WORK" "$SONY_AS" "$PSPDEV_BIN" "$HOST_TOOLS"; do
  case $v in /*) ;; *) die "paths must be absolute (got '$v'); set PSPDEV_BIN" ;; esac
done
[ $# -gt 0 ] || die "usage: sh build.sh BUILD... (4.0.2h 4.0.2x 4.0.4h 4.0.4x 4.0.4c 4.1.0s, or all)"

# A clean, fixed host environment: the recorded builds were made with exactly these flags and no others.
unset CPPFLAGS LDFLAGS LIBS CPP CONFIG_SITE GCC_EXEC_PREFIX COMPILER_PATH LIBRARY_PATH CPATH C_INCLUDE_PATH \
      CPLUS_INCLUDE_PATH MAKEFLAGS MFLAGS 2>/dev/null || true
LC_ALL=C; LANG=C; export LC_ALL LANG
umask 022
PATH=$HOST_PATH; export PATH
# Old GCC needs the old C dialect on a modern host compiler; the GMP_RND* aliases are only in the 4.1.0s build's
# flags (they are what that build was configured with; GCC 4.1.0 itself does not use MPFR).
HF0="-O1 -g0 -std=gnu89 -fcommon -fpermissive -w -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0"
HFGMP="-DGMP_RNDN=MPFR_RNDN -DGMP_RNDZ=MPFR_RNDZ -DGMP_RNDU=MPFR_RNDU -DGMP_RNDD=MPFR_RNDD"
CXXF="-O1 -g0 -std=gnu++98 -fpermissive -w"

spec() {  # sets VER PATCHF REJ HF BASE DIFFS for build $1
  BASE= DIFFS= PATCHF= REJ= HF=$HF0
  case $1 in
    4.0.2h) VER=4.0.2 PATCHF=65aaa94632-gcc-4.0.2.patch ;;
    4.0.4h) VER=4.0.4 PATCHF=65aaa94632-gcc-4.0.2.patch REJ=gcc/version.c.rej ;;
    4.1.0s) VER=4.1.0 PATCHF=7be303fec4-gcc-4.1.0-PSP.patch HF="$HF0 $HFGMP" ;;
    4.0.2x) VER=4.0.2 BASE=4.0.2h DIFFS=lbp-allegrex-tuning.diff ;;
    4.0.4x) VER=4.0.4 BASE=4.0.4h DIFFS="lbp-allegrex-tuning.diff lbp-ins-from-zero-4.0.4.diff lbp-allegrex-xfer-4.0.4.diff" ;;
    4.0.4c) VER=4.0.4 BASE=4.0.4h DIFFS="lbp-allegrex-tuning.diff lbp-ins-from-zero-4.0.4.diff lbp-allegrex-xfer-4.0.4.diff lbp-copy-order-asm-slot-4.0.4.diff" ;;
    *) die "unknown build '$1' (want 4.0.2h 4.0.2x 4.0.4h 4.0.4x 4.0.4c 4.1.0s or all)" ;;
  esac
}

check_tree() {  # check_tree TAG SRC_DIR: the patched source files against trees/psp-gcc-TAG.sha256
  t=$HERE/trees/psp-gcc-$1.sha256
  if [ -f "$t" ]; then
    (cd "$2" && sha256sum -c --quiet "$t") || die "$1: patched sources differ from trees/psp-gcc-$1.sha256"
    echo "TREE_OK psp-gcc-$1 ($(wc -l < "$t") patched files as recorded)"
  else
    echo "TREE_UNCHECKED psp-gcc-$1 (no trees/psp-gcc-$1.sha256)"
  fi
}

report() {  # report TAG
  (cd "$ROOT" && sha256sum "psp-gcc-$1/bin/psp-gcc" psp-gcc-$1/libexec/gcc/psp/*/cc1 \
     psp-gcc-$1/libexec/gcc/psp/*/cc1plus psp-gcc-$1/libexec/gcc/psp/*/collect2) | sed 's/^/SHA256 /'
  echo "BUILD_OK psp-gcc-$1 $("$ROOT/psp-gcc-$1/bin/psp-gcc" --version | head -1)"
}

build_base() {  # an h/s build: GNU tarball + pspdev patch, configured with the Sony assembler
  tag=$1; P=$ROOT/psp-gcc-$tag; W=$WORK/psp-gcc-$tag
  echo "== psp-gcc-$tag: GCC $VER + $PATCHF"
  rm -rf "$W" "$P"; mkdir -p "$W" "$P/bin"
  for t in as ld objdump objcopy; do ln -s "$PSPDEV_BIN/psp-$t" "$P/bin/psp-$t"; done
  for t in nm ar ranlib strip; do ln -s "$HOST_TOOLS/$t" "$P/bin/psp-$t"; done
  cd "$W"
  tar --no-same-owner -xjf "$DL/gcc-$VER.tar.bz2"
  cd "gcc-$VER"
  # -F3: the 2007 patch was written for 4.0.2 and applies to 4.0.4 with fuzz; its version.c hunk does not apply
  # there (so 4.0.4h reports plain "4.0.4"). The expected rejects are checked, then every patched file's hash.
  patch -p1 -F3 -N < "$DL/$PATCHF" > ../patch.log 2>&1 || true
  got=$(find . -name '*.rej' | sed 's|^\./||' | sort | tr '\n' ' ' | sed 's/ $//')
  [ "$got" = "$REJ" ] || { cat ../patch.log; die "$tag: patch rejects '$got', expected '$REJ'"; }
  check_tree "$tag" "$W/gcc-$VER"
  mkdir ../build; cd ../build
  # The driver searches its own bin/ for psp-* tools: configure runs with the install's bin/ first on PATH.
  PATH=$P/bin:$HOST_PATH MAKEINFO=true CC=gcc CFLAGS="$HF" CXX=g++ CXXFLAGS="$CXXF" \
    "../gcc-$VER/configure" --target=psp --prefix="$P" --enable-languages=c,c++ --disable-nls --disable-libssp \
    --with-newlib --without-headers --disable-shared --disable-threads --disable-multilib --disable-werror \
    --with-as="$SONY_AS" --with-ld="$PSPDEV_BIN/psp-ld" > configure.log 2>&1 || { tail -20 configure.log; die "$tag configure"; }
  # libgcc cannot be built (the Sony assembler rejects the lwl/lwr homebrew GCC emits for it): all-gcc stops there,
  # after the compiler proper exists; -k builds everything else. Only the compiler proper is installed.
  PATH=$P/bin:$HOST_PATH make -k -j"$JOBS" all-gcc MAKEINFO=true > make.log 2>&1 || true
  [ -x gcc/cc1 ] && [ -x gcc/cc1plus ] && [ -x gcc/xgcc ] || { tail -30 make.log; die "$tag: no cc1/cc1plus"; }
  (cd gcc && PATH=$P/bin:$HOST_PATH make install-driver install-common MAKEINFO=true > ../install.log 2>&1) \
    || { tail -20 install.log; die "$tag install"; }
  report "$tag"
}

build_x() {  # an x build: the h work tree + this project's changes, cc1/cc1plus rebuilt in place
  tag=$1; P=$ROOT/psp-gcc-$tag; W=$WORK/psp-gcc-$tag; B=$WORK/psp-gcc-$BASE
  if [ ! -x "$B/build/gcc/cc1plus" ] || [ ! -x "$ROOT/psp-gcc-$BASE/bin/psp-gcc" ]; then
    ( build "$BASE" )
    spec "$tag"
  fi
  echo "== psp-gcc-$tag: psp-gcc-$BASE + $DIFFS + allegrex-sched.md"
  rm -rf "$W" "$P"; cp -a "$B" "$W"
  cd "$W/gcc-$VER"
  for d in $DIFFS; do
    patch -p1 -F0 --no-backup-if-mismatch < "$HERE/patches/$d" > "../patch-$d.log" 2>&1 \
      || { cat "../patch-$d.log"; die "$tag: $d does not apply"; }
  done
  cp "$HERE/patches/allegrex-sched.md" gcc/config/mips/allegrex-sched.md
  check_tree "$tag" "$W/gcc-$VER"
  cd ../build/gcc
  make -j"$JOBS" cc1 cc1plus MAKEINFO=true CFLAGS="$HF" > ../make-$tag.log 2>&1 \
    || { tail -20 "../make-$tag.log"; die "$tag make"; }
  cp -a "$ROOT/psp-gcc-$BASE" "$P"
  cp cc1 cc1plus "$P/libexec/gcc/psp/$VER/"
  report "$tag"
}

build() { spec "$1"; if [ -n "$BASE" ]; then build_x "$1"; else build_base "$1"; fi; }

# Inputs
[ -n "$PSPDEV_BIN" ] || die "set PSPDEV_BIN to a pspdev toolchain's bin/ directory (see README.md)"
for t in ld as objdump objcopy; do [ -x "$PSPDEV_BIN/psp-$t" ] || die "missing $PSPDEV_BIN/psp-$t"; done
if [ ! -e "$SONY_AS" ] && [ "$SONY_AS" = "$ROOT/sony-as" ]; then
  mkdir -p "$ROOT"; cp "$HERE/sony-as" "$SONY_AS"; chmod 755 "$SONY_AS"; echo "installed wrapper $SONY_AS"
fi
"$SONY_AS" --version 2>/dev/null | grep -qF "$SONY_AS_VERSION" \
  || die "$SONY_AS does not run the Sony assembler $SONY_AS_VERSION (see README.md, 'The assembler')"
echo "host: $(gcc --version | head -1); $(patch --version | head -1); $(make --version | head -1)"
echo "pspdev: $("$PSPDEV_BIN/psp-ld" --version | head -1)"
sh "$HERE/fetch.sh" "$DL"
mkdir -p "$WORK"

[ "$*" = all ] && set -- 4.0.2h 4.0.2x 4.0.4h 4.0.4x 4.0.4c 4.1.0s
for b; do ( build "$b" ); done
echo "ALL_DONE $*"
