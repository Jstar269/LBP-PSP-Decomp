/* LBPMATCH module=EBOOT.BIN start=0x0898c364 end=0x0898c408 symbol=FUN_0898c364 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Node {
  const void *vt;
  short size;
  short rc;
  int x;
  int a;
  int b;
  int c;
  char m[16];
};
extern char DAT_08bbfad8[];
extern void *FUN_08a9e838(int n);
extern void FUN_0898c03c(void *dst, void *src);

struct Node **FUN_0898c364(struct Node **out, int x, int *src)
{
  struct Node *n = (struct Node *)FUN_08a9e838(40);
  int a, b;
  n->x = x;
  a = src[0];
  b = src[1];
  n->c = src[2];
  n->size = -1;
  n->rc = 1;
  n->vt = DAT_08bbfad8;
  n->a = a;
  n->b = b;
  FUN_0898c03c(n->m, src + 3);
  n->size = 40;
  *out = n;
  return out;
}
// NEAR: 26 words differ (40/41, one word short). Node factory like 0x0898c210 plus a trailing call FUN_0898c03c(node + 24, src + 12) (wip/FUN_0898c03c.c, the M copy constructor).
// Retail copies the three words src[0..2] into the member at +12 through a hoisted `addiu v1,s0,12` (c stored `sw v0,8(v1)` first, a with `sw a2,12(s0)`), and passes src + 12 / node + 24 prepared before the header stores;
// ours stores through s0 and prepares the call arguments later. Tried: struct W3 copy through a pointer, int-array copy with Ghidra's statement order.
// near miss from lane api-lbp-b-20261121 (unreviewed)
