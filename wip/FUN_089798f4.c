/* LBPMATCH module=EBOOT.BIN start=0x089798f4 end=0x08979994 symbol=FUN_089798f4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Rc { char pad[6]; short rc; };
struct Node {
  const void *vt;
  short size;
  short rc;
  int a;
  int b;
  struct Rc *r;
};
extern char DAT_08bbd920[];
extern void *DAT_08be84a0;
extern struct Node *FUN_08adbb84(void *pool, int size);

struct Node **FUN_089798f4(struct Node **out, int a, int *src)
{
  struct Node *n = FUN_08adbb84(DAT_08be84a0, 20);
  if (n != 0) {
    struct Rc *r;
    int b;
    n->a = a;
    b = src[0];
    r = (struct Rc *)src[1];
    n->size = -1;
    n->rc = 1;
    n->vt = DAT_08bbd920;
    n->b = b;
    n->r = r;
    if (r != 0)
      r->rc = r->rc + 1;
    n->size = 20;
  }
  *out = n;
  return out;
}
// NEAR: 4 words differ (40/40): only the order of the two halfword header stores (retail `li v0,1; sh v0,6; li v0,-1; sh v0,4`, ours size first). The natural load order (src[1] before src[0] as in Ghidra) and an int-array view of src fixed the rest.
// Either source order of the two stores gives the same code.
// near miss from lane api-lbp-b-20261121 (unreviewed)
