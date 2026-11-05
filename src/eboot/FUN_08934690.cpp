/* LBPMATCH module=EBOOT.BIN start=0x08934690 end=0x08934704 symbol=FUN_08934690 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08931c30(void *p);
extern "C" void *FUN_08b898a0(void *n);
extern "C" void FUN_088ef420(void *l);
struct Item { int pad[13]; int f52; };
struct Node { int pad[3]; int pad2; int pad3; Item *item; };
struct List { int a; int sentinel; int b; Node *first; int c; int count; };
struct Obj { int pad[139]; List l; };
extern "C" void FUN_08934690(Obj *o)
{
  List *l = &o->l;
  if (l->count) {
    Node *n = l->first;
    while (n != (Node *)&l->sentinel) {
      Item *it = n->item;
      it->f52 = 9;
      FUN_08931c30(it);
      n = (Node *)FUN_08b898a0(n);
    }
    FUN_088ef420(l);
  }
}
