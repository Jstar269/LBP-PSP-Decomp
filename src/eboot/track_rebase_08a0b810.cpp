/* LBPMATCH module=EBOOT.BIN start=0x08a0b810 end=0x08a0b864 symbol=FUN_08a0b810 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Ref {
  int pad[0x1c / 4];
  float base; /* +0x1c */
};
struct Item;
struct Track {
  int pad[2];
  Ref *ref;     /* +0x8 */
  int pad2[(0xb8 - 0xc) / 4];
  float offset; /* +0xb8 */
};
extern "C" void FUN_08a0b14c(Track *track, Item *item, bool flag);
extern "C" float FUN_08805198(Item *item);

extern "C" void FUN_08a0b810(Track *track, Item *item, bool flag)
{
  FUN_08a0b14c(track, item, flag);
  track->offset = FUN_08805198(item) - track->ref->base + track->offset;
}
