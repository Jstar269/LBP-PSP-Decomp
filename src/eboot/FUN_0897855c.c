/* LBPMATCH module=EBOOT.BIN start=0x0897855c end=0x08978658 symbol=FUN_0897855c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Item;
struct IVT { int pad; void (*del)(struct Item *); };
struct Item { const struct IVT *vt; };
struct Ref;
struct RVT { void (*dtor)(struct Ref *); };
struct Ref { const struct RVT *vt; int count; };
struct Self {
  const void *vt;
  int pad[5];
  struct Item **items;
  int count;
  int pad1[2];
  struct Ref *r40;
  struct Ref *r44;
};
extern char DAT_08bbd880[];
extern char DAT_08bab5b8[];
extern void FUN_08a9e0c4(void *p);
extern void FUN_08a9e0cc(void *p);
extern void FUN_08adb8a4(void *p);

void FUN_0897855c(struct Self *self)
{
  int i;
  self->vt = DAT_08bbd880;
  for (i = 0; i < self->count; i++) {
    struct Item *it = self->items[i];
    if (it != 0)
      it->vt->del(it);
  }
  if (self->items != 0)
    FUN_08a9e0c4(self->items);
  {
    struct Ref *p = self->r44;
    if (p != 0) {
      int n = p->count - 1;
      p->count = n;
      if (n == 0) {
        p->vt->dtor(p);
        FUN_08adb8a4(p);
      }
    }
  }
  {
    struct Ref *p = self->r40;
    if (p != 0) {
      int n = p->count - 1;
      p->count = n;
      if (n == 0) {
        p->vt->dtor(p);
        FUN_08adb8a4(p);
      }
    }
  }
  self->vt = DAT_08bab5b8;
  FUN_08a9e0cc(self);
}
