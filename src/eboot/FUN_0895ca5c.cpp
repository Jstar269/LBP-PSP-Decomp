/* LBPMATCH module=EBOOT.BIN start=0x0895ca5c end=0x0895ca74 symbol=FUN_0895ca5c lang=c++ */
/* Clone of src/eboot/FUN_088f7e58.cpp (lbpclone.py tag b2): addresses substituted; exact match verified; names and types await human review. */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" char DAT_08bb80d8[];
struct P { void *vt; int f4, f8; };
extern "C" void FUN_0895ca5c(struct P *o)
{
  o->vt = DAT_08bb80d8;
  o->f4 = 0;
  o->f8 = 0;
}
