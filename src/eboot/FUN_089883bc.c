/* LBPMATCH module=EBOOT.BIN start=0x089883bc end=0x089884b8 symbol=FUN_089883bc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Pl { char pad[68]; int kind; };
struct Ev { char pad[6]; short rc; int type; struct Pl *pl; };
struct Self { char pad[28]; int type; struct Pl *f32; struct Pl *f36; struct Pl *f40; };
extern void FUN_08a3bf90(struct Pl *p);
extern void FUN_08a3cd58(struct Pl *p);
extern void FUN_08a65658(struct Pl *p);
extern void FUN_089745b8(struct Self *s, struct Ev **ev);
extern void FUN_08970b6c(struct Ev *e);

void FUN_089883bc(struct Self *self, struct Ev **pe)
{
  struct Ev *e = *pe;
  struct Ev *tmp;
  int t = e->type;
  self->type = t;
  if ((unsigned)(t - 15001) < 2) {
    struct Pl *pl = e->pl;
    int k = pl->kind;
    self->f32 = pl;
    if (k == 9)
      self->f36 = pl;
    else if (k == 28)
      self->f40 = pl;
  }
  switch (self->type) {
  case 15001:
    FUN_08a3bf90(self->f36);
    e = *pe;
    break;
  case 15002:
    if (self->f36 != 0) {
      FUN_08a3cd58(self->f36);
      e = *pe;
    } else if (self->f40 != 0) {
      FUN_08a65658(self->f40);
      e = *pe;
    } else {
      e = *pe;
    }
    break;
  default:
    e = *pe;
    break;
  }
  tmp = e;
  if (e != 0)
    e->rc = e->rc + 1;
  FUN_089745b8(self, &tmp);
  if (tmp != 0)
    FUN_08970b6c(tmp);
}
