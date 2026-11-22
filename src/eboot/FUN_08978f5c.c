/* LBPMATCH module=EBOOT.BIN start=0x08978f5c end=0x08978ff0 symbol=FUN_08978f5c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Item;
struct IVT { int pad; void (*del)(struct Item *); };
struct Item { const struct IVT *vt; };
struct Self { const void *vt; int pad[3]; struct Item **items; int count; };
extern char DAT_08bbd8b8[];
extern char DAT_08bab5b8[];
extern void FUN_08a9e0c4(void *p);
extern void FUN_08a9e0cc(void *p);

void FUN_08978f5c(struct Self *self)
{
  int i;
  self->vt = DAT_08bbd8b8;
  for (i = 0; i < self->count; i++) {
    struct Item *it = self->items[i];
    if (it != 0)
      it->vt->del(it);
  }
  if (self->items != 0)
    FUN_08a9e0c4(self->items);
  self->vt = DAT_08bab5b8;
  FUN_08a9e0cc(self);
}
