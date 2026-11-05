# EXACT only under the proposed fifth Allegrex change (no mtc1/mfc1 transfer delay)

These five near misses are EXACT (lbpmatch, 2026-11-04 ~09:49Z) when psp-gcc-4.0.4x is replaced by the 4.0.4w
build: work-4.0.4z (the adopted ins-from-zero build) with `ISA_HAS_XFER_DELAY (mips_isa <= 3 && !TARGET_ALLEGREX)`
(now `toolchain/gcc4/patches/lbp-allegrex-xfer-4.0.4.diff`; cc1 c1e0e28bc0479600, cc1plus 165786919b1a23af). They stay MISMATCH
under the current kit, so they are not installed in src/ and must not be promoted until the owner adopts the change
and `lbppromote.py migrate-toolchain` has run.

Evidence for the change (retail EBOOT, all ledger functions): `mtc1` then a reader of its FPR: 718 sites, with a nop
between: 0; `mfc1` then a reader: 215, with a nop: 0; FP compare then bc1: 0 direct, 1,752 with a nop (the FCMP
delay is real and stays). The three src/ sources built by gcc404x that contain mtc1/mfc1 (FUN_08a3fe70,
subtract_and_trigger_0897a550, timer_expire_0897f384) stay EXACT under 4.0.4w.

Reproduce: `LBP_GCC4_HOME='$HOME/lbp-gcc4-opus' python tools/lbpmatch.py wip/xfer/*.c wip/xfer/*.cpp`
(~/lbp-gcc4-opus symlinks every kit of ~/lbp-gcc4, with psp-gcc-4.0.4x -> psp-gcc-4.0.4w).

**Adopted 2026-11-04.** The owner approved the change after lane B's independent re-match (VERDICT: ADOPT,
reports/v002-plan/reviews/lbp-404w-rematch.md). 4.0.4w is now the installed 4.0.4x kit; the old kit is kept as
`psp-gcc-4.0.4x.stock-20261104`. `migrate-toolchain` reverified all 6,151 proofs. The sources from this folder are now in
src/eboot together with eight others, and all twelve have been promoted (journal event 130).
