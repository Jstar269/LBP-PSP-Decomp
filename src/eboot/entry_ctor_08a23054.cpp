/* LBPMATCH module=EBOOT.BIN start=0x08a23054 end=0x08a230b8 symbol=_ZN14Entry_08a23054C1EPi lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Range {
  Range() : kind(0), a(-1), b(-1), c(-1), d(-1), e(-1) {}
  unsigned char kind;
  signed char a, b, c, d, e;
};
struct Entry_08a23054 {
  Entry_08a23054(int *owner);
  int *owner;     /* +0x0 */
  int id;         /* +0x4 */
  int z0, z1, z2, z3, z4, z5; /* +0x8 */
  Range range;    /* +0x20 */
  unsigned char on; /* +0x26 */
};

Entry_08a23054::Entry_08a23054(int *owner_) : owner(owner_), id(*owner_), z0(0), z1(0), z2(0), z3(0), z4(0), z5(0)
{
  range.a = -1;
  range.b = -1;
  range.c = -1;
  range.d = -1;
  range.e = -1;
  on = 1;
}
