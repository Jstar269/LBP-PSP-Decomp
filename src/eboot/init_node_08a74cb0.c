/* LBPMATCH module=EBOOT.BIN start=0x08a74cb0 end=0x08a74cf4 symbol=FUN_08a74cb0 lang=c99 */
/* Clone of src/eboot/init_node_08a31dc8.c (lbpclone.py tag d1): addresses substituted; exact match verified; names and types await human review. */
/* Hand-written reconstruction from the disassembly (lane fb-near1, Claude); names and types await human review. */
extern int DAT_08bc7fd8;
extern void FUN_08a31c30(void *, int);
extern void FUN_08a556e8(void *);

typedef struct Node {
  struct Node *next;
  struct Node *prev;
} Node;

void FUN_08a74cb0(void **p, int a)
{
  Node *node = (Node *)(p + 39);

  FUN_08a31c30(p, a);
  p[0] = &DAT_08bc7fd8;
  node->next = node;
  node->prev = node;
  p[45] = 0;
  FUN_08a556e8(p + 47);
}
