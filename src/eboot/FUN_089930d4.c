/* LBPMATCH module=EBOOT.BIN start=0x089930d4 end=0x08993198 symbol=FUN_089930d4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Ev { char pad[8]; int type; int arg; };
struct Self { char pad[16]; int f16; };
extern void *DAT_08be8510;
extern int FUN_0880c414(void *w);
extern void FUN_0880bae0(void *w, int a);
extern void FUN_08992e04(struct Self *s, int a);

int FUN_089930d4(struct Self *self, struct Ev **pe)
{
  void *w = DAT_08be8510;
  struct Ev *e = *pe;
  int r;
  switch (e->type) {
  case 5000: {
    int a = e->arg;
    if (FUN_0880c414(w) != 0) {
      FUN_0880bae0(w, a);
      r = 0;
    } else {
      FUN_0880bae0(w, a);
      r = 0;
      self->f16 = 13;
    }
    break;
  }
  case 5008: {
    int a = e->arg;
    int t = FUN_0880c414(w);
    r = 0;
    if (t == 0) {
      FUN_08992e04(self, a);
      r = 0;
    }
    break;
  }
  default:
    r = 1;
    break;
  }
  return r;
}
