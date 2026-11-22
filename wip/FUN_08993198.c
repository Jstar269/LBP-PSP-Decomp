/* LBPMATCH module=EBOOT.BIN start=0x08993198 end=0x08993294 symbol=FUN_08993198 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Pl { int flag; int x; int y; };
struct Ev { char pad[8]; int type; struct Pl pl; };
struct Self { char pad[16]; int f16; char pad1[8]; int f28; int f32; unsigned char f36; };
struct Obj { char pad[6]; short rc; };
extern void *DAT_08be8510;
extern void FUN_0880f278(void *w, int a, int b, int c);
extern void FUN_08992e04(struct Self *s, int a);
extern void FUN_0880b148(void *w, int a);
extern void FUN_089745b8(struct Self *s, struct Obj **o);
extern void FUN_08970b6c(struct Obj *o);

void FUN_08993198(struct Self *self, struct Ev **pe)
{
  int a = 1;
  struct Ev *e = *pe;
  struct Pl *pl = &e->pl;
  struct Obj *o;
  if (e->type == 7000) {
    int f;
    self->f28 = e->type;
    f = e->pl.flag != 0;
    if (!f)
      a = 2;
    self->f36 = f;
    self->f32 = pl->x;
    FUN_0880f278(DAT_08be8510, a, pl->y, pl->x);
    self->f16 = 13;
  } else if (e->type == 7001) {
    self->f28 = e->type;
    self->f36 = e->pl.flag != 0;
    self->f32 = pl->x;
    FUN_08992e04(self, pl->y);
  }
  FUN_0880b148(DAT_08be8510, 1);
  o = (struct Obj *)*pe;
  {
    struct Obj *tmp = o;
    if (o != 0)
      o->rc = o->rc + 1;
    FUN_089745b8(self, &tmp);
    if (tmp != 0)
      FUN_08970b6c(tmp);
  }
}
// NEAR: 25 words differ (63/63). The if/else-if chain on the event type (7000 then 7001) and the payload pointer `ev + 12` match; retail keeps the event pointer in a1 and the pointer a = 1 / movz t0,v0,v1 value in t0 from the start (`li t0,1` before the loads),
// ours loads the event into a2, hoists `li a1,2` and uses different registers for the payload fields. Tried: switch (49-59 words), declaration order of `a`.
// near miss from lane api-lbp-b-20261121 (unreviewed)
