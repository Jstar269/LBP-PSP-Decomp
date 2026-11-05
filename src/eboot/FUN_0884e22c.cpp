/* LBPMATCH module=EBOOT.BIN start=0x0884e22c end=0x0884e28c symbol=FUN_0884e22c lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" unsigned FUN_08882758(void);
extern "C" void FUN_0884e0cc(unsigned a, unsigned b);
struct Rec { int pad[12]; int f48, f52, f56, f60, f64, f68; };
struct Ctx { int pad[3]; unsigned id; int pad2[12]; Rec *rec; /* +0x40 */ };
extern "C" bool FUN_0884e22c(int *state, void *unused, Ctx *c)
{
  bool done = false;
  if (*state == 0) {
    Rec *r = c->rec;
    unsigned id = c->id;
    r->f60 = r->f48;
    r->f64 = r->f52;
    r->f68 = r->f56;
    FUN_0884e0cc(id, FUN_08882758());
    done = true;
  }
  return done;
}
