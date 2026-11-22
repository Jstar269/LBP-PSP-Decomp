/* LBPMATCH module=EBOOT.BIN start=0x088741f0 end=0x088742a4 symbol=FUN_088741f0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17a, Codex GPT-6); exact match verified; names and types await human review. */
typedef unsigned int u32;
typedef struct Node {
  struct Node *next;
  struct Node *prev;
  void *value;
} Node;

extern char *DAT_08be84ac;
extern void FUN_08a9e0c4(void *);
extern void FUN_08ad3088(void *);
extern void FUN_08adbc24(void *, void *, u32);
extern void FUN_08ad2810(void *);

void FUN_088741f0(char *param_1)
{
  Node *sentinel = (Node *)(param_1 + 0x10180);
  Node *node;

  for (node = sentinel->next; node != sentinel; node = node->next) {
    if (node->value != 0) {
      FUN_08a9e0c4(node->value);
    }
  }

  node = sentinel->next;
  while (node != sentinel) {
    Node *next = node->next;
    char *heap = DAT_08be84ac;
    FUN_08ad3088(heap + 32);
    FUN_08adbc24(heap, node, 0x10);
    FUN_08ad2810(heap + 32);
    node = next;
  }

  sentinel->next = sentinel;
  sentinel->prev = sentinel;
}
