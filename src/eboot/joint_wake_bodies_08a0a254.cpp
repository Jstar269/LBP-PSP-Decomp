/* LBPMATCH module=EBOOT.BIN start=0x08a0a254 end=0x08a0a2f8 symbol=FUN_08a0a254 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Body {
  unsigned int bits0 : 3;
  unsigned int dynamic3 : 1;
};
struct Joint {
  void *vtable;
  Body *a; /* +0x4 */
  Body *b; /* +0x8 */
  char pad[0x54 - 0xc];
  void *ownerA; /* +0x54 */
  void *ownerB; /* +0x58 */
  char pad2[0x64 - 0x5c];
  unsigned int bit0 : 1;
  unsigned int bit1 : 1;
  unsigned int driven : 1;
};
extern "C" bool FUN_08a74610(void *owner);
extern "C" void FUN_08a10024(Body *body);

extern "C" void FUN_08a0a254(Joint *joint)
{
  if (joint->a->dynamic3 && !FUN_08a74610(joint->ownerA))
    FUN_08a10024(joint->a);
  if (joint->b->dynamic3 && !FUN_08a74610(joint->ownerB))
    FUN_08a10024(joint->b);
  joint->driven = 0;
}
