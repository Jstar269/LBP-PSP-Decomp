/* LBPMATCH module=EBOOT.BIN start=0x08978190 end=0x08978240 symbol=FUN_08978190 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj { char pad[6]; short rc; };
struct Self { char pad[24]; void **items; int count; };
extern int FUN_08979e88(void *item, struct Obj **ref);
extern void FUN_08970b6c(struct Obj *o);

void FUN_08978190(struct Self *self, struct Obj **pref)
{
  int i;
  for (i = 0; i < self->count; i++) {
    void *item = self->items[i];
    if (item != 0) {
      struct Obj *tmp = *pref;
      int r;
      if (tmp != 0)
        tmp->rc = tmp->rc + 1;
      r = FUN_08979e88(item, &tmp);
      if (tmp != 0)
        FUN_08970b6c(tmp);
      if (r == 0)
        return;
    }
  }
}
