/* LBPMATCH module=EBOOT.BIN start=0x0898cda8 end=0x0898ce5c symbol=FUN_0898cda8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct It { int pad; unsigned flags; };
struct Ctx {
  char pad0[40];
  struct It **base_a;
  short s44;
  char pad1[98];
  int **base_c;
  short s148;
  char pad2[10];
  int **base_b;
  short s164;
};
struct Self { char pad0[20]; struct Ctx *ctx; char pad1[8]; int f32; };
extern void *DAT_08befcac;
extern int FUN_089dee6c(int *p);
extern void FUN_0895cce4(void *a, int b);

void FUN_0898cda8(struct Self *self)
{
  struct Ctx *c = self->ctx;
  int *d = c->base_c[-c->s148];
  int *b = c->base_b[-c->s164];
  struct It *a = c->base_a[4 - c->s44];
  if (self->f32 == 1 && FUN_089dee6c(d) == (int)b && (a->flags & 4) != 0)
    FUN_0895cce4(DAT_08befcac, 34);
}
