/* LBPMATCH module=EBOOT.BIN start=0x0885f314 end=0x0885f3ec symbol=FUN_0885f314 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17g, GPT-6); exact match verified; names and types await human review. */
typedef struct Ref { void **vtable; int references; } Ref;
typedef struct Node {
  int pad[2];
  struct Node *left;
  struct Node *right;
  int field10;
  Ref *ref;
  int field18;
} Node;
extern char DAT_08bef8f4[];
extern char *DAT_08be84ac;
extern void FUN_08ad3088(void *), FUN_08ad2810(void *);
extern void FUN_08adbc24(void *, void *, unsigned);

void FUN_0885f314(void *self, Node *n)
{
  while (n) {
    Ref *r;
    Node *next;
    char *heap;
    FUN_0885f314(self, n->right);
    r = n->ref;
    next = n->left;
    if (r) {
      int count;
      FUN_08ad3088(DAT_08bef8f4);
      count = --r->references;
      FUN_08ad2810(DAT_08bef8f4);
      if (!count)
        ((void (*)(Ref *))r->vtable[1])(r);
    }
    heap = DAT_08be84ac;
    FUN_08ad3088(heap + 32);
    FUN_08adbc24(heap, n, 28);
    FUN_08ad2810(heap + 32);
    n = next;
  }
}
