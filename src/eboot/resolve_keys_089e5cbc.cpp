/* LBPMATCH module=EBOOT.BIN start=0x089e5cbc end=0x089e5d70 symbol=FUN_089e5cbc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Elem { int f0; int f4; int f8; char pad[24]; };
struct Vec { Elem *begin; int pad4; int count; int pad12[3]; };
struct Obj {
  char pad0[36];
  Vec a;
  Vec b;
  int table;
};
extern "C" void FUN_0898d758(void *buf, int key);
extern "C" int FUN_089e5bd0(int *table, void *buf);
inline Vec *sel(Obj *o, int i) { return i == 0 ? &o->a : &o->b; }
extern "C" void FUN_089e5cbc(Obj *o)
{
  char buf[80];
  for (int i = 0; i < 2; i++) {
    Elem *first = sel(o, i)->begin;
    Elem *end = first + sel(o, i)->count;
    for (Elem *e = first; e < end; e++) {
      FUN_0898d758(buf, e->f4);
      e->f8 = FUN_089e5bd0(&o->table, buf);
    }
  }
}
