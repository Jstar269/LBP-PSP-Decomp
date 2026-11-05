/* LBPMATCH module=EBOOT.BIN start=0x088a1800 end=0x088a1818 symbol=FUN_088a1800 lang=c99 cc=gcc171-O2 */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct P { char pad[20]; int *arr; };
void FUN_088a1800(struct P *o, int i, int *q)
{
  int *p = o->arr + i;
  *p = *q;
}
