/* LBPMATCH module=EBOOT.BIN start=0x0886b948 end=0x0886ba38 symbol=FUN_0886b948 lang=c99 cc=gcc404xs-Os */
/* Hand-written reconstruction from the disassembly (lane api-haiku-20261121, claude-haiku-5-5); exact match verified; names and types await human review. */
extern int *DAT_08be84ac;
extern int DAT_08bef8f4;

struct Ref {
  int pad;
  int count;
};

struct Item {
  int a;
  int b;
  struct Ref *r;
};

struct Node {
  struct Node *next;
  struct Node *prev;
  struct Item item;
};

void FUN_08ad3088(int *lock);
void FUN_08ad2810(int *lock);
struct Node *FUN_08adbb84(int *heap, unsigned int n);
void FUN_08b88fac(struct Node *node, struct Node *head);

void FUN_0886b948(struct Node *param_1, struct Node *param_2)
{
  struct Node *cur;
  struct Node *n;
  struct Item *si;
  struct Ref *r;

  param_1->next = param_1;
  cur = param_2->next;
  param_1->prev = param_1;
  for (; cur != param_2; cur = cur->next) {
    int *lock = DAT_08be84ac + 8;
    si = &cur->item;
    FUN_08ad3088(lock);
    n = FUN_08adbb84(DAT_08be84ac, 20);
    FUN_08ad2810(lock);
    if (&n->item != 0) {
      r = si->r;
      n->item.b = si->b;
      n->item.a = cur->item.a;
      n->item.r = r;
      if (r != 0) {
        FUN_08ad3088(&DAT_08bef8f4);
        r->count = r->count + 1;
        FUN_08ad2810(&DAT_08bef8f4);
      }
    }
    FUN_08b88fac(n, param_1);
  }
}
// NEAR: lbpmatch 30 words differ (60 compared). Matches: the head self-links, the loop with the test at the bottom,
// the lock/alloc/unlock sequence and the refcount increment. Still differs: the saved-register roles (retail:
// this=s4, cur=s3, source item=s5, new node=s2, lock=s1; ours permuted), and the field store order (retail r,a,b;
// ours b,a,r) which is not changed by reversing the source statements.
// Ruled out: the source-item pointer (si = &cur->item) gives the retail load pattern (r via si, a via cur, b via si).
// near miss from lane api-haiku-20261121 (unreviewed)
