/* LBPMATCH module=EBOOT.BIN start=0x089ea928 end=0x089ea93c symbol=FUN_089ea928 lang=c++ */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Member {
  int value;
};
struct Holder {
  void *const *vtable;
  Member member; /* +0x4 */
};
extern "C" void *const DAT_08bc2770[];
extern "C" void FUN_08a1fb64(Member *member);

extern "C" void FUN_089ea928(Holder *holder)
{
  holder->vtable = DAT_08bc2770;
  FUN_08a1fb64(&holder->member);
}
