# Main's variants of functions both branches matched (reconciliation of 2026-11-22)

`main` and `research/60fps` diverged at a5ea074 and each matched 75 of the same functions independently, with
different sources (same address, same range, both EXACT and promoted on their own branch). The accepted ledger has one
owner per byte, so the merge keeps research/60fps's source in `src/` (its proof is the accepted one) and keeps main's
source here, under its original path, so no work is lost: 16 had the same file name on both branches (add/add), 59 a
different one. Each is a complete, EXACT-on-main source; it can replace the `src/` one through `lbppromote.py` if it is
preferred (better names or types).
