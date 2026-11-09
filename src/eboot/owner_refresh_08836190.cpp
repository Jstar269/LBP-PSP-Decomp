/* LBPMATCH module=EBOOT.BIN start=0x08836190 end=0x08836334 symbol=FUN_08836190 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp19o, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Node_08836190;
struct Owner_08836190;
struct Query_08836190 {
  int count;
  Node_08836190 *n0, *n1, *n2, *n3, *n4, *n5, *n6, *n7;
  int key0, key1;
  Node_08836190 **out[8];
};
struct Owner_08836190 {
  int pad0;
  char header[0x14];  /* +0x04 */
  char *found;        /* +0x18 */
  int key0;           /* +0x1c */
  int key1;           /* +0x20 */
  bool dirty;         /* +0x24 */
};
extern "C" char *FUN_0882dab8(Owner_08836190 *o, int *key);
extern "C" void FUN_0882ff40(Owner_08836190 *o, Node_08836190 *n);
extern "C" void FUN_0882fe34(Owner_08836190 *o, Query_08836190 *q);
extern "C" void FUN_08833478(Owner_08836190 *o, void *arg);
extern "C" void FUN_088308a0(Owner_08836190 *o);
extern "C" void FUN_08835930(Owner_08836190 *o);

extern "C" bool FUN_08836190(Owner_08836190 *o, void *arg)
{
  if (!o->dirty)
    return false;
  char *it = FUN_0882dab8(o, &o->key0);
  o->found = it;
  if (it != o->header)
    FUN_0882ff40(o, *(Node_08836190 **)(it + 0x18));
  Query_08836190 q;
  q.out[0] = &q.n6;
  q.out[1] = &q.n3;
  q.out[2] = &q.n7;
  q.out[3] = &q.n1;
  q.out[4] = &q.n5;
  q.out[5] = &q.n2;
  q.out[6] = &q.n4;
  q.out[7] = &q.n0;
  q.key0 = o->key0;
  q.key1 = o->key1;
  q.count = 0;
  q.n0 = 0;
  q.n1 = 0;
  q.n2 = 0;
  q.n3 = 0;
  q.n4 = 0;
  q.n5 = 0;
  q.n6 = 0;
  q.n7 = 0;
  FUN_0882fe34(o, &q);
  if (q.n0) FUN_0882ff40(o, q.n0);
  if (q.n2) FUN_0882ff40(o, q.n2);
  if (q.n1) FUN_0882ff40(o, q.n1);
  if (q.n3) FUN_0882ff40(o, q.n3);
  if (q.n4) FUN_0882ff40(o, q.n4);
  if (q.n5) FUN_0882ff40(o, q.n5);
  if (q.n6) FUN_0882ff40(o, q.n6);
  if (q.n7) FUN_0882ff40(o, q.n7);
  FUN_08833478(o, arg);
  FUN_088308a0(o);
  FUN_08835930(o);
  o->dirty = false;
  return true;
}
