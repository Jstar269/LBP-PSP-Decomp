/* LBPMATCH module=EBOOT.BIN start=0x089e0850 end=0x089e0914 symbol=FUN_089e0850 lang=c99 rodata=0x08badc9c */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Inner { short v[2]; };
struct Item { int pad[3]; struct Inner in; int pad2; void *obj; };
struct Holder { int pad0[3]; struct Item *cur; struct Item *prev; };
extern void FUN_08a9e0cc(void *p);
extern void *FUN_08a9e838(int size);
extern void FUN_089e1a14(float a, float b, void *obj, int n, struct Inner *in);
extern void FUN_089e1b90(float a, float b, void *obj, int n, struct Inner *in);

void FUN_089e0850(struct Holder *h, struct Item *n)
{
  struct Item *old = h->cur;
  h->cur = n;
  h->prev = old;
  if (old != 0) {
    FUN_08a9e0cc(old->obj);
    old->obj = 0;
  }
  {
    struct Item *c = h->cur;
    if (c->obj != 0) {
      FUN_089e1a14(1.5f, 1.5f, c->obj, 4, &c->in);
      return;
    }
    {
      void *o = FUN_08a9e838(80);
      FUN_089e1b90(1.5f, 1.5f, o, 4, &c->in);
      c->obj = o;
    }
  }
}
