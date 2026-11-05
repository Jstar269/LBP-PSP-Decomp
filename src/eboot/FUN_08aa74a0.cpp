/* LBPMATCH module=EBOOT.BIN start=0x08aa74a0 end=0x08aa74bc symbol=FUN_08aa74a0 lang=c++ */
/* Clone of src/eboot/FUN_08aa6d84.cpp (lbpclone.py tag b2): addresses substituted; exact match verified; names and types await human review. */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" char DAT_08bab530[];
extern "C" char DAT_08bc3820[];
struct P { void *a; int pad[2]; void *d; };
extern "C" void FUN_08aa74a0(struct P *o)
{
  o->d = DAT_08bc3820;
  o->a = DAT_08bab530;
}
