/* LBPMATCH module=EBOOT.BIN start=0x0898cc8c end=0x0898cd28 symbol=FUN_0898cc8c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Self { const void *vt; int pad[3]; void **items; int count; };
extern char DAT_08bbfb08[];
extern void FUN_08978ff0(struct Self *s, int a, int b, int c, int d);
extern void *FUN_0898abc8(struct Self *s);
extern void FUN_08978f04(struct Self *s, void *p);
extern void FUN_08978114(void *item, int a, int b);

void FUN_0898cc8c(struct Self *self, int a, int c, int d)
{
  int i;
  FUN_08978ff0(self, a, 1, c, d);
  self->vt = DAT_08bbfb08;
  FUN_08978f04(self, FUN_0898abc8(self));
  for (i = 0; i < self->count; i++) {
    void *it = self->items[i];
    if (it != 0)
      FUN_08978114(it, 2, 3);
  }
}
