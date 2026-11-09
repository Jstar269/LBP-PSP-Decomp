/* LBPMATCH module=EBOOT.BIN start=0x08a4c488 end=0x08a4c4fc symbol=_ZN12Obj_08a4c488C1EPvS0_ lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lane lbp19o, Claude Opus 5.5); exact match verified; names and types await human review. */
extern "C" void *memset(void *p, int v, unsigned int n);

struct ObjBase_08a4c488 {
  int field_04;
  char field_08[20];
  int field_1c;
  ObjBase_08a4c488() : field_04(0), field_1c(0) { memset(field_08, 0, sizeof field_08); }
  virtual ~ObjBase_08a4c488();
};

struct Obj_08a4c488 : ObjBase_08a4c488 {
  Obj_08a4c488(void *a, void *b);
  void FUN_08a4bda8(void *a, void *b);
  virtual ~Obj_08a4c488();
};

Obj_08a4c488::Obj_08a4c488(void *a, void *b) { FUN_08a4bda8(a, b); }
