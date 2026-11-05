/* LBPMATCH module=EBOOT.BIN start=0x0889b800 end=0x0889b864 symbol=FUN_0889b800 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" void FUN_0889b0cc(void *p);
extern "C" void FUN_0889b014(void *a, void *n, void *x);
extern void *DAT_08bb1990[];
struct Node { void **vt; int pad[2]; char flag; };
extern "C" void FUN_0889b800(void *a, void *x)
{
  Node *n = (Node *)FUN_08a9e838(16);
  FUN_0889b0cc(n);
  n->vt = DAT_08bb1990;
  n->flag = 0;
  FUN_0889b014(a, n, x);
}
