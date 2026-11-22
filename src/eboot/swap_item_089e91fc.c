/* LBPMATCH module=EBOOT.BIN start=0x089e91fc end=0x089e92d0 symbol=FUN_089e91fc lang=c99 rodata=0x08badc9c */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Inner { short v[2]; };
struct Item { int pad[3]; struct Inner in; int pad2; void *obj; };
struct Holder { char pad0[64]; struct Item *cur; };
extern void *FUN_08a9e838(int size);
extern void FUN_089e1a14(float a, float b, void *obj, int n, struct Inner *in);
extern void FUN_089e1b90(float a, float b, void *obj, int n, struct Inner *in);
extern void *FUN_08914a7c(void);
extern int FUN_08914e94(void *p, int v);
extern void FUN_08913dd8(void *p, int a, int b, int c, int d, int e, int f, int g);
extern int DAT_08bef3f0;

void FUN_089e91fc(struct Holder *h)
{
  struct Item *c = h->cur;
  void *p1, *p2;
  if (c->obj != 0) {
    FUN_089e1a14(1.5f, 1.5f, c->obj, 4, &c->in);
  } else {
    void *o = FUN_08a9e838(80);
    FUN_089e1b90(1.5f, 1.5f, o, 4, &c->in);
    c->obj = o;
  }
  p1 = FUN_08914a7c();
  p2 = FUN_08914a7c();
  FUN_08913dd8(p1, 0, FUN_08914e94(p2, DAT_08bef3f0), 0, 0, 0, 0x400, 0);
}
