/* LBPMATCH module=EBOOT.BIN start=0x0899f4c0 end=0x0899f584 symbol=FUN_0899f4c0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

extern "C" {
void FUN_088050b8(void *p);
void FUN_088a827c(void *p);
void FUN_08a9bff8(void *p);
void FUN_08a9e0cc(void *p);
extern void *DAT_08bc0528[];
extern void *DAT_08babc90[];
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
};
struct Arr {
  Elem e[3];
};
struct Obj {
  void **vt;
  char pad[28];
  Arr arr;
};

extern "C" void FUN_0899f4c0(Obj *self)
{
  self->vt = DAT_08bc0528;
  self->arr.~Arr();
  self->vt = DAT_08babc90;
}
