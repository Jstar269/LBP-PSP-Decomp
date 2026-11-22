/* LBPMATCH module=EBOOT.BIN start=0x08962b48 end=0x08962c30 symbol=FUN_08962b48 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj;
struct VT { int a; void (*release)(struct Obj *); };
struct Obj { const struct VT *vt; };
struct Holder { struct Obj *a; struct Obj *b; };
extern void *DAT_08bf03b8;
extern void *DAT_08be85f8;
extern void FUN_0896ee30(void *o);
extern int FUN_0896ed78(void *o, int a);
extern void FUN_0896f03c(void);
extern void *FUN_0896f064(void *o);
extern void FUN_08998850(void *p);
extern void FUN_08968924(void *o, int a);
extern void FUN_0896f07c(void *o, int a, void *p);

void FUN_08962b48(struct Holder *h)
{
  FUN_0896ee30(DAT_08bf03b8);
  if (FUN_0896ed78(DAT_08bf03b8, 0) != 0) {
    FUN_0896f03c();
    FUN_08998850(FUN_0896f064(DAT_08bf03b8));
  }
  FUN_08968924(DAT_08be85f8, 1);
  if (h->a != 0) {
    FUN_0896f07c(DAT_08bf03b8, 0, h->a);
    if (h->a != 0)
      h->a->vt->release(h->a);
  }
  if (h->b != 0) {
    FUN_0896f07c(DAT_08bf03b8, 0, h->b);
    if (h->b != 0)
      h->b->vt->release(h->b);
  }
}
