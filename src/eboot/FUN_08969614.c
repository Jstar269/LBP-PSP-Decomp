/* LBPMATCH module=EBOOT.BIN start=0x08969614 end=0x089696b4 symbol=FUN_08969614 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Rec { int a; int b; int pad; unsigned c; };
extern void *DAT_08bf0380;
extern void FUN_089e174c(void *pool, int *p, int v);
extern int FUN_08a9e824(unsigned v);

struct Hdr {
  char pad0[8];
  int pad1[8];
  int idx;
  int pad2;
  unsigned cur;
  int x34;
  int x38;
};

void FUN_08969614(struct Hdr *self, unsigned v)
{
  struct Rec *r;
  int t;
  if (v == self->cur)
    return;
  r = (struct Rec *)((char *)self + 8 + self->idx * 16);
  FUN_089e174c(DAT_08bf0380, &r->b, r->a);
  r->c = v;
  t = 0;
  if (v != 0)
    t = FUN_08a9e824(v & 0xfffffffc);
  int i2 = self->idx;
  r->b = t;
  t = ((struct Rec *)((char *)self + 8 + i2 * 16))->b;
  self->cur = v;
  self->x34 = t;
  self->x38 = t;
}
