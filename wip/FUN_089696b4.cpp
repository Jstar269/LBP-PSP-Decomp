/* LBPMATCH module=EBOOT.BIN start=0x089696b4 end=0x089697b4 symbol=FUN_089696b4 lang=c++ */
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
extern "C" void FUN_089696b4(Obj *self)
{
  self->~Obj();
  FUN_08a9e0cc(self);
}
// NEAR: 68 words differ (72/64 words). The first half matches (vtable store, four `if (p) FUN_08a9e0c4(p)` handle members destroyed in reverse order, the `&slots != 0` guard,
// tail call to FUN_08a9e0cc); the member-array loop (2 slots of 16 bytes at +8, count 3 down) strength-reduces five induction variables (s1..s5, s6 counter, s7 hi) where retail uses
// s1 (element, used for a, &b, &c), s2/s3 (zero-store bases), s4 counter, s5 hi. Same member-array-destructor problem as lbp9a's 0x0896cd08.
// Tried: real C++ member destructors for Slot and Handle (this form).
// near miss from lane api-lbp-b-20261121 (unreviewed)
