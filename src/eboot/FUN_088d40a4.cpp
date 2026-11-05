/* LBPMATCH module=EBOOT.BIN start=0x088d40a4 end=0x088d40b8 symbol=FUN_088d40a4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
typedef unsigned int u32;
struct S { u32 f0; char pad[104]; int f108; };
extern "C" void FUN_088d40a4(struct S *o)
{
  o->f108 = 0;
  o->f0 &= ~0x200000u;
}
