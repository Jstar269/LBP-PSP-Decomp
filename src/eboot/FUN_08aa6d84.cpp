/* LBPMATCH module=EBOOT.BIN start=0x08aa6d84 end=0x08aa6da0 symbol=FUN_08aa6d84 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" char DAT_08bab530[];
extern "C" char DAT_08bc3820[];
struct P { void *a; int pad[2]; void *d; };
extern "C" void FUN_08aa6d84(struct P *o)
{
  o->d = DAT_08bc3820;
  o->a = DAT_08bab530;
}
