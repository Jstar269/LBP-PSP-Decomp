/* LBPMATCH module=EBOOT.BIN start=0x089ea650 end=0x089ea694 symbol=FUN_089ea650 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Member {
  int value;
};
struct Holder {
  void *const *vtable;
  Member member; /* +0x4 */
};
extern "C" void *const DAT_08bc2770[];
extern "C" void *const DAT_08bc27a0[];
extern "C" void FUN_08a1fb94(Member *member);
extern "C" void FUN_08a9e0cc(void *p);

extern "C" void FUN_089ea650(Holder *holder)
{
  holder->vtable = DAT_08bc2770;
  FUN_08a1fb94(&holder->member);
  holder->vtable = DAT_08bc27a0;
  FUN_08a9e0cc(holder);
}
