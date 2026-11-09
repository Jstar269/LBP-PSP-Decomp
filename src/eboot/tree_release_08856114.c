/* LBPMATCH module=EBOOT.BIN start=0x08856114 end=0x088561ec symbol=FUN_08856114 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17g, GPT-6); exact match verified; names and types await human review. */
struct Ref { void **vtable; int references; };
struct Node {
  int pad[2];
  struct Node *left;
  struct Node *right;
  int field10;
  int field14;
  struct Ref *ref;
  int field1c;
};
extern char DAT_08bef8f4[];
extern char *DAT_08be84ac;
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_08adbc24(void *, void *, unsigned);

void FUN_08856114(void *self, struct Node *node)
{
  while (node) {
    struct Ref *ref;
    struct Node *next;
    char *heap;
    FUN_08856114(self, node->right);
    ref = node->ref;
    next = node->left;
    if (ref) {
      int count;
      FUN_08ad3088(DAT_08bef8f4);
      count = --ref->references;
      FUN_08ad2810(DAT_08bef8f4);
      if (!count)
        ((void (*)(struct Ref *))ref->vtable[2])(ref);
    }
    heap = DAT_08be84ac;
    FUN_08ad3088(heap + 32);
    FUN_08adbc24(heap, node, 32);
    FUN_08ad2810(heap + 32);
    node = next;
  }
}
