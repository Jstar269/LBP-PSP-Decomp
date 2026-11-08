/* LBPMATCH module=EBOOT.BIN start=0x089e8ee0 end=0x089e8f70 symbol=FUN_089e8ee0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane fb-near1, Claude); names and types await human review. */
extern int DAT_08be84a0;
extern void FUN_0896dddc(void *);
extern void FUN_08adbc24(int, void *, int);

typedef struct Node {
  struct Node *next;
  struct Node *prev;
  void *value;
} Node;

void FUN_089e8ee0(int *param_1)
{
  Node *head = (Node *)(param_1 + 0x1a);
  Node *node;

  for (node = head->next; node != head; node = node->next) {
    int *value = (int *)node->value;

    if (value != 0)
      (*(void (**)(int *))(*value + 4))(value);
  }
  node = head->next;
  while (node != head) {
    Node *next = node->next;

    FUN_08adbc24(DAT_08be84a0, node, 0xc);
    node = next;
  }
  head->next = head;
  head->prev = head;
  FUN_0896dddc(param_1);
}
