/* LBPMATCH module=EBOOT.BIN start=0x0889a460 end=0x0889a4c0 symbol=FUN_0889a460 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08baa244(void *p, int a, int b);
extern "C" void FUN_08aeda54(void);
extern "C" void FUN_08baa1e4(void *p, int a);
extern void **DAT_08bee228;
struct Obj { char pad[11]; unsigned char flag; };
extern "C" void FUN_0889a460(Obj *o)
{
  if (o->flag == 0) {
    FUN_08baa244(*DAT_08bee228, 1, 0);
    FUN_08aeda54();
    FUN_08baa1e4(*DAT_08bee228, 1);
  }
}
