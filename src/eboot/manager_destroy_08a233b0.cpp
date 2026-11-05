/* LBPMATCH module=EBOOT.BIN start=0x08a233b0 end=0x08a23430 symbol=FUN_08a233b0 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Elem_08a29b84 {
  ~Elem_08a29b84();
  int words[44];
};
extern "C" void *DAT_08bef190;
struct Base {
  ~Base() { DAT_08bef190 = 0; }
};
struct Manager : Base {
  Elem_08a29b84 elems[9];
};
extern "C" Manager *DAT_08bf5e80;
extern "C" void FUN_08a274d8(Manager *manager);

extern "C" void FUN_08a233b0(void)
{
  FUN_08a274d8(DAT_08bf5e80);
  delete DAT_08bf5e80;
  DAT_08bf5e80 = 0;
}
