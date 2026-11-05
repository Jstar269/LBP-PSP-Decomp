/* LBPMATCH module=EBOOT.BIN start=0x08a9f84c end=0x08a9f898 symbol=FUN_08a9f84c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08b89838(void *n);
struct N { char pad[20]; int v; };
extern N *DAT_08bf930c;
extern char DAT_08bf9304[];
extern "C" int FUN_08a9f84c()
{
  N *p = DAT_08bf930c;
  while (p != (N *)DAT_08bf9304) {
    if (p->v) return p->v;
    p = (N *)FUN_08b89838(p);
  }
  return 0;
}
