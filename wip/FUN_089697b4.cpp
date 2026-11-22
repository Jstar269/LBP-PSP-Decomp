/* LBPMATCH module=EBOOT.BIN start=0x089697b4 end=0x089698a8 symbol=FUN_089697b4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
extern char DAT_08bb8898[];
extern void *DAT_08bf0380;
extern "C" void FUN_08a9e0c4(void *);
extern "C" void FUN_08a9e0cc(void *);
extern "C" void FUN_089e174c(void *, void *, int);
extern "C" void FUN_089e16ac(void *, void *, int);
struct Slot {
  int a;
  void *b;
  void *c;
  int d;
  ~Slot()
  {
    FUN_089e174c(DAT_08bf0380, &b, a);
    FUN_089e16ac(DAT_08bf0380, &c, a);
    b = 0;
    c = 0;
  }
};
struct Handle {
  void *p;
  int pad[2];
  ~Handle()
  {
    if (p)
      FUN_08a9e0c4(p);
  }
};
struct Obj {
  const void *vt;
  int pad0;
  Slot slots[2];
  char pad1[24];
  Handle h64;
  Handle h76;
  Handle h88;
  Handle h100;
  ~Obj() { vt = DAT_08bb8898; }
};
extern "C" void FUN_089697b4(Obj *self)
{
  self->~Obj();
}
// NEAR: 64 words differ (69/61 words). Same body as 0x089696b4 (wip/FUN_089696b4.cpp) without the deleting tail call; retail reuses s0 (this) as a zero-store induction variable.
// Member-array destructor loop: ours strength-reduces extra induction variables for the `a` loads (16(s3), 16(s2)) where retail loads through the element pointer s1 (0(s1)). Not resolved by real C++ member destructors.
// near miss from lane api-lbp-b-20261121 (unreviewed)
