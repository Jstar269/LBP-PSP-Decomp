/* LBPMATCH module=EBOOT.BIN start=0x089f1a64 end=0x089f1a94 symbol=FUN_089f1a64 lang=c++ */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
extern "C" int DAT_08bf3788;   /* depth */
extern "C" int DAT_08bdbe54[]; /* stack */
extern "C" int DAT_08bf92e0;   /* current */

extern "C" void FUN_089f1a64(int id)
{
  DAT_08bdbe54[++DAT_08bf3788] = id;
  DAT_08bf92e0 = id;
}
