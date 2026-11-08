/* LBPMATCH module=EBOOT.BIN start=0x088adb5c end=0x088adb88 symbol=FUN_088adb5c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane fb-near1, Claude); names and types await human review. */
typedef struct Node {
  struct Node *next;
  struct Node *prev;
} Node;

void FUN_088adb5c(int *param_1)
{
  Node *node = (Node *)(param_1 + 3);

  node->next = node;
  node->prev = node;
  param_1[0] = 0;
  *(short *)(param_1 + 1) = 0;
  param_1[2] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  *(short *)(param_1 + 7) = 0;
  *(short *)((int)param_1 + 0x1e) = 0;
}
