/* LBPMATCH module=EBOOT.BIN start=0x0896ec3c end=0x0896ed20 symbol=FUN_0896ec3c lang=c99 rodata=0x08bab59c */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Anim {
  unsigned cur;
  unsigned pressed;
  int pad[1];
  unsigned char done;
  char pad1[3];
  float t;
  unsigned u;
  float f24;
  float f28;
  float f32;
};
struct Pad { unsigned pad; unsigned buttons; };
struct Ent { unsigned bit; unsigned pad; };
extern struct Ent DAT_08bdb868[];
extern unsigned DAT_08bf0398;
extern void FUN_0896e968(struct Anim *a, unsigned *out, struct Pad *p);

void FUN_0896ec3c(struct Anim *a, struct Pad *p)
{
  unsigned mask = 0;
  unsigned x;
  unsigned n;
  unsigned i;
  const struct Ent *e = DAT_08bdb868;
  for (i = 1; i != 20; i++, e++) {
    if ((p->buttons & (1 << e->bit)) != 0)
      mask |= 1 << i;
  }
  x = 0;
  FUN_0896e968(a, &x, p);
  n = mask | x;
  a->pressed = n & ~a->cur;
  if (a->pressed != 0) {
    a->u = 0;
    a->done = 0;
    a->t = a->f32 + 1.0f;
  }
  if (a->done != 0) {
    unsigned g = DAT_08bf0398;
    a->done = 0;
    a->pressed = a->pressed | (a->cur & g);
  }
  a->cur = n;
}
