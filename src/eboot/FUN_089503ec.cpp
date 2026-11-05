/* LBPMATCH module=EBOOT.BIN start=0x089503ec end=0x08950404 symbol=FUN_089503ec lang=c++ */
/* Clone of src/eboot/FUN_088f7e58.cpp (lbpclone.py tag b2): addresses substituted; exact match verified; names and types await human review. */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" char DAT_08bb7ac8[];
struct P { void *vt; int f4, f8; };
extern "C" void FUN_089503ec(struct P *o)
{
  o->vt = DAT_08bb7ac8;
  o->f4 = 0;
  o->f8 = 0;
}
