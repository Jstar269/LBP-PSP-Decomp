/* LBPMATCH module=EBOOT.BIN start=0x0898c210 end=0x0898c2a0 symbol=FUN_0898c210 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Node {
  const void *vt;
  short size;
  short rc;
  int x;
  int a;
  int b;
  int c;
};
extern char DAT_08bbfaf8[];
extern void *DAT_08be84a0;
extern struct Node *FUN_08adbb84(void *pool, int size);

struct Node **FUN_0898c210(struct Node **out, int x, int *src)
{
  struct Node *n = FUN_08adbb84(DAT_08be84a0, 24);
  if (n != 0) {
    int c, a, b;
    n->x = x;
    a = src[0]; b = src[1]; c = src[2];
    n->rc = 1;
    n->vt = DAT_08bbfaf8;
    n->a = a;
    n->b = b;
    n->c = c;
    n->size = 24;
  }
  *out = n;
  return out;
}
