/* LBPMATCH module=EBOOT.BIN start=0x08998cc8 end=0x08998d7c symbol=FUN_08998cc8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Ref;
struct Item;
struct Rs {
  int a;
  unsigned short pad;
  unsigned short count;
  int kind;
};
struct Mgr {
  char pad[144];
  Item **arr;
  short n;
};
struct Self {
  char pad[20];
  Mgr *mgr;
};

extern "C" {
void FUN_08970b6c(Rs *p);
void FUN_089de274(Item *i, int k);
void FUN_089745b8(Self *s, const Ref &r);
}

struct Ref {
  Rs *p;
  Ref(const Ref &o) : p(o.p) {
    if (p) p->count = p->count + 1;
  }
  ~Ref() { if (p) FUN_08970b6c(p); }
};

extern "C" void FUN_08998cc8(Self *self, const Ref *r)
{
  Rs *o = r->p;
  Item *it = self->mgr->arr[1 - self->mgr->n];
  switch (o->kind) {
  case 4004:
    FUN_089de274(it, 1);
    break;
  case 13000:
    FUN_089de274(it, 2);
    break;
  }
  FUN_089745b8(self, Ref(*r));
}
