/* LBPMATCH module=EBOOT.BIN start=0x089df808 end=0x089df8a0 symbol=FUN_089df808 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Node {
  const void *vt;
  short size : 16;
  short rc : 16;
  int x;
  int a;
  int b;
  int c;
};
extern char DAT_08bc1ba0[];
extern void *DAT_08be84a0;
extern struct Node *FUN_08adbb84(void *pool, int size);

struct Node **FUN_089df808(struct Node **out, int x, int *src)
{
  struct Node *n = FUN_08adbb84(DAT_08be84a0, 24);
  if (n != 0) {
    int c, a, b;
    n->size = -1;
    n->rc = 1;
    a = src[0]; b = src[1]; c = src[2];
    n->vt = DAT_08bc1ba0;
    n->x = x;
    n->a = a;
    n->b = b;
    n->c = c;
    n->size = 24;
  }
  *out = n;
  return out;
}
// NEAR 5 words: only s1/s2 roles of out/x swapped (retail out=s2,x=s1). Plain short fields lose the dead `sh -1` store (DSE); `short:16` bit-fields keep it.
// Tried: C++ base ctor, struct-copy, sret Handle, inline helper, x stored earlier: none swaps the registers.
// near miss from lane api-lbp-d-20261121 (unreviewed)
