/* LBPMATCH module=EBOOT.BIN start=0x088a183c end=0x088a1854 symbol=FUN_088a183c lang=c99 cc=gcc171-O2 */
/* Clone of src/eboot/FUN_088a1800.c (lbpclone.py tag b2): addresses substituted; exact match verified; names and types await human review. */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct P { char pad[20]; int *arr; };
void FUN_088a183c(struct P *o, int i, int *q)
{
  int *p = o->arr + i;
  *p = *q;
}
