/* LBPMATCH module=EBOOT.BIN start=0x088742a4 end=0x08874360 symbol=FUN_088742a4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17a, Codex GPT-6); exact match verified; names and types await human review. */
typedef unsigned int u32;
typedef struct Node {
  struct Node *next;
  struct Node *prev;
  void *value;
} Node;

extern char DAT_08bb05c8[];
extern char *DAT_08be84ac;
extern void FUN_088741f0(void *);
extern void FUN_08878b78(int *);
extern void FUN_08ad3088(void *);
extern void FUN_08adbc24(void *, void *, u32);
extern void FUN_08ad2810(void *);
extern void FUN_08873a2c(void *);
extern void FUN_08a9e0cc(void *);

void FUN_088742a4(int *param_1)
{
  Node *sentinel;
  Node *node;

  param_1[0] = (u32)DAT_08bb05c8;
  FUN_088741f0(param_1);
  FUN_08878b78(param_1 + 0x4063);
  sentinel = (Node *)(param_1 + 0x4060);
  node = sentinel->next;
  while (node != sentinel) {
    Node *next = node->next;
    char *heap = DAT_08be84ac;
    FUN_08ad3088(heap + 32);
    FUN_08adbc24(heap, node, 0x10);
    FUN_08ad2810(heap + 32);
    node = next;
  }
  FUN_08873a2c(param_1);
  FUN_08a9e0cc(param_1);
}
