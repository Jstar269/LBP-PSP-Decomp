/* LBPMATCH module=EBOOT.BIN start=0x089ac34c end=0x089ac424 symbol=FUN_089ac34c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

extern "C" {
void FUN_088050b8(void *p);
void FUN_088a827c(void *p);
void FUN_08a9bff8(void *p);
void FUN_08962538(void *p);
void FUN_089ac178(void *self, int v, void *e);
void FUN_08970b6c(void *p);
}

struct Ref {
  void *p;
  ~Ref() { if (p) FUN_088050b8(p); }
};
struct Ref2 {
  void *p;
  ~Ref2() { if (p) FUN_088a827c(p); }
};
struct Sub {
  char data[24];
  ~Sub() { FUN_08a9bff8(this); }
};
struct Elem {
  Sub s;
  Ref a;
  Ref b;
  Ref c;
  int pad;
  Ref2 d;
  int pad2;
  Elem() { FUN_08962538(this); }
};

struct Rs {
  int a;
  unsigned short pad;
  unsigned short count;
  int kind;
  int pad2;
  int f16;
};

struct RefH {
  Rs *p;
  RefH(const RefH &o) : p(o.p) {
    if (p) p->count = p->count + 1;
  }
  ~RefH() { if (p) FUN_08970b6c(p); }
};

extern "C" int FUN_089ac34c(void *self, const RefH *r)
{
  if (r->p->kind == 13003) {
    int v = r->p->f16;
    Elem e;
    FUN_089ac178(self, v, &e);
  }
  RefH t(*r);
  return 1;
}
