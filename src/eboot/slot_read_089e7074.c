/* LBPMATCH module=EBOOT.BIN start=0x089e7074 end=0x089e7130 symbol=FUN_089e7074 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Hdr { int a, b, c; };
struct Ent { struct Hdr h; int d[16]; };
struct Table { int pad0; int count; struct Ent *ents; };
struct Slot { int pad0[6]; struct Table *tab; int state; };
struct Out { struct Hdr h; int d[16]; };
extern void FUN_08ba0e04(int *dst, int *src);

int FUN_089e7074(char *m, int idx, int i, struct Out *out, int *cnt)
{
  int off = idx * 32 + 144;
  struct Slot *s = (struct Slot *)(m + off);
  int r = 1;
  if (s->tab != 0) {
    struct Table *t;
    if (i != -1234) {
      struct Ent *e = &s->tab->ents[i];
      if (out != 0) {
        out->h = e->h;
        FUN_08ba0e04(out->d, e->d);
      }
    }
    t = s->tab;
    r = 0;
    *cnt = t->count;
  } else {
    if ((unsigned)(s->state - 1) < 2)
      r = 1;
    else
      r = 4;
  }
  return r;
}
