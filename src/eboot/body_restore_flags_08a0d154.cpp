/* LBPMATCH module=EBOOT.BIN start=0x08a0d154 end=0x08a0d1c4 symbol=FUN_08a0d154 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp19o, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Bits_08a0d154 {
  unsigned b0:1; unsigned b1:1; unsigned b2:1; unsigned b3:1; unsigned b4:1; unsigned b5:1; unsigned b6:1;
  unsigned b7:1; unsigned b8:1; unsigned b9:1; unsigned b10:1; unsigned rest:21;
};
struct Obj_08a0d154 { char pad[40]; Bits_08a0d154 saved; char pad2[24]; Bits_08a0d154 live; char pad3[20]; Bits_08a0d154 f5c; };
extern "C" void FUN_08a0d154(Obj_08a0d154 *o)
{
  Bits_08a0d154 s = o->saved;
  Bits_08a0d154 l = o->live;
  l.b0 = s.b0;
  l.b1 = s.b1;
  l.b2 = s.b2;
  l.b3 = s.b3;
  l.b4 = s.b4;
  l.b5 = s.b5;
  l.b6 = s.b6;
  l.b7 = s.b7;
  l.b9 = s.b8;
  l.b10 = s.b9;
  l.b8 = s.b10;
  o->live = l;
  o->f5c.b0 = 0;
}
