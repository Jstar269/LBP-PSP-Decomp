/* LBPMATCH module=EBOOT.BIN start=0x0896e628 end=0x0896e6c4 symbol=FUN_0896e628 lang=c99 rodata=0x08bab59c */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Anim {
  unsigned flags;
  int pad[2];
  unsigned char done;
  char pad1[3];
  float t;
  float u;
  float f24;
  float f28;
  float f32;
  float f36;
};
extern unsigned DAT_08bf0398;

void FUN_0896e628(float dt, struct Anim *o)
{
  float u, lim, t;
  if ((o->flags & DAT_08bf0398) != 0) {
    u = o->u + dt;
    o->u = u;
    if (o->f36 < u) {
      lim = o->f32;
    } else {
      if (!(u > o->f28))
        return;
      lim = o->f24;
    }
    t = o->t + dt;
    o->t = t;
    if (lim < t) {
      o->done = 1;
      o->t = 0;
    }
  } else {
    o->done = 0;
    o->u = 0;
    o->t = o->f32 + 1.0f;
  }
}
