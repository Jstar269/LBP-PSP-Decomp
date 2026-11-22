/* LBPMATCH module=EBOOT.BIN start=0x08964ac4 end=0x08964b88 symbol=FUN_08964ac4 lang=c99 rodata=0x08babd28 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Link { struct Link *next; };
struct Sub { char pad[564]; struct Link head; };
struct G { int a; int pad; int b; int pad2; int pad3; struct Sub *sub; };
struct W { char pad[1148]; float f; };
struct Out { int pad; int code; };
extern struct G *DAT_08bf5e44;
extern struct W *DAT_08be8510;

int FUN_08964ac4(struct Out *out)
{
  struct G *g = DAT_08bf5e44;
  int r = 0;
  int ok;
  int code;
  if (g == 0)
    return r;
  ok = 1;
  if (DAT_08be8510 != 0) {
    unsigned n = 0;
    struct Link *p;
    ok = DAT_08be8510->f < 0.999f;
    if (g->sub != 0) {
      for (p = g->sub->head.next; p != &g->sub->head; p = p->next)
        n++;
      if (n >= 100)
        goto L5;
    }
  }
  if (g->a != 0 && g->sub != 0) {
    r = 1;
    code = 143;
    if (ok)
      goto L3;
  }
L5:
  r = 0;
  if (g->a != 0)
    return r;
  if (g->b == 0)
    return r;
  r = 1;
  code = 155;
L3:
  out->code = code;
  return r;
}
// NEAR: 16 words differ (49/49). Control flow now matches retail only through two gotos (L5 shared failure tail, L3 shared store); the plain nested-if form is 25-39 words off
// (movz if-conversion of the n >= 100 flag, or a different block order). Remaining: retail keeps the return value in v1 (`move v0,v1` at a single exit) and the code in v0 (143/155),
// ours puts the code in v1 and returns through v0. Tried: explicit int r with r=0/1 assignments (same code), declaration order.
// near miss from lane api-lbp-b-20261121 (unreviewed)
