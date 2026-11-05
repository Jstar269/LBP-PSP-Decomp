/* LBPMATCH module=EBOOT.BIN start=0x08a0d114 end=0x08a0d154 symbol=FUN_08a0d114 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Flags_08a0d114 { unsigned b0:1; unsigned low:7; unsigned b8:1; unsigned b9:1; unsigned b10:1; unsigned rest:21; };
struct Obj_08a0d114 { char pad[40]; unsigned saved; char pad2[24]; unsigned live; char pad3[20]; Flags_08a0d114 f5c; };
extern "C" void FUN_08a0d114(Obj_08a0d114 *o)
{
  unsigned v = o->live;
  o->saved = (v & 0xfe) | (((v >> 9) & 1) << 8) | (((v >> 10) & 1) << 9) | (((v >> 8) & 1) << 10);
  o->f5c.b0 = 0;
}
