/* LBPMATCH module=EBOOT.BIN start=0x088bb9d0 end=0x088bba30 symbol=FUN_088bb9d0 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Cfg { int pad0; int pad[6]; int mode; /* +28 */ int pad2[136]; int extra; /* +576 */ };
struct Base { int pad[3]; Cfg cfg; };
struct Holder { Base *base; };
struct Rec { int pad[22]; unsigned char flags; char pad2[3]; int extra; };
extern "C" void FUN_088bb9d0(Holder *h, Rec *r)
{
  Cfg *c = &h->base->cfg;
  switch (c->mode) {
  case 0:
    r->flags |= 2;
    r->extra = c->extra;
    break;
  case 1:
    r->flags |= 4;
    break;
  case 2:
    r->flags |= 8;
    break;
  }
}
