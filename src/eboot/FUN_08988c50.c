/* LBPMATCH module=EBOOT.BIN start=0x08988c50 end=0x08988ce8 symbol=FUN_08988c50 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Tg;
struct TVT { int pad[57]; void (*fe4)(struct Tg *); };
struct Tg { const struct TVT *vt; };
struct Ev { char pad[6]; short rc; char pad1[4]; struct Tg *tg; unsigned char flag; };
struct Self { char pad[20]; int f20; char pad1[4]; struct Tg *tg; };
extern void FUN_089745b8(struct Self *s, struct Ev **ev);
extern void FUN_08970b6c(struct Ev *e);
extern void FUN_089a8560(struct Tg *t, int a, int b);

void FUN_08988c50(struct Self *self, struct Ev **pe)
{
  struct Ev *tmp = *pe;
  struct Tg *t;
  unsigned char f;
  if (tmp != 0)
    tmp->rc = tmp->rc + 1;
  FUN_089745b8(self, &tmp);
  if (tmp != 0)
    FUN_08970b6c(tmp);
  t = (*pe)->tg;
  f = (*pe)->flag;
  self->tg = t;
  FUN_089a8560(t, self->f20, f);
  if (t != 0)
    t->vt->fe4(t);
}
