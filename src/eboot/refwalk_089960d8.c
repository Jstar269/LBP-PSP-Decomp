/* LBPMATCH module=EBOOT.BIN start=0x089960d8 end=0x08996194 symbol=FUN_089960d8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Rc { int a; unsigned short pad; unsigned short count; };
struct Node;
struct VT { int a; int b; int (*fn)(struct Node *, struct Rc **); };
struct Node { struct VT *vt; int b; struct Node *next; };
struct Holder { int a; int b; struct Node *head; };

extern void FUN_0880f20c(struct Rc **out, int a, void *b);
extern void FUN_08970b6c(struct Rc *r);
extern void FUN_08974264(struct Holder *h);

void FUN_089960d8(struct Holder *h)
{
  int x;
  struct Rc *t;
  struct Rc *r;
  struct Node *n;
  int res;

  FUN_0880f20c(&r, 0xfc1, &x);
  for (n = h->head; n != 0; n = n->next) {
    int (*fn)(struct Node *, struct Rc **);
    t = r;
    fn = n->vt->fn;
    if (r != 0)
      r->count = r->count + 1;
    res = fn(n, &t);
    if (t != 0)
      FUN_08970b6c(t);
    if (res == 0)
      break;
  }
  if (r != 0)
    FUN_08970b6c(r);
  FUN_08974264(h);
}
