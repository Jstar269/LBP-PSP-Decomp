/* LBPMATCH module=EBOOT.BIN start=0x08a131c0 end=0x08a132a8 symbol=FUN_08a131c0 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Obj_08a131c0;
struct Hub_08a131c0;
struct Rec_08a131c0 { int a; int b; int e; int d; int c; };
struct World_08a131c0 { char pad[8204]; Rec_08a131c0 *recs; unsigned count; char pad2[80]; unsigned char compacting; };
extern "C" Hub_08a131c0 *FUN_08a50fbc(void);
extern "C" Obj_08a131c0 *FUN_08a52b38(Hub_08a131c0 *hub, int id);
extern "C" void FUN_08a12ec0(World_08a131c0 *w, Obj_08a131c0 *a, Obj_08a131c0 *b, int c, int d, int e);
extern "C" void FUN_08a131c0(World_08a131c0 *w)
{
  if (w->recs == 0)
    return;
  for (unsigned i = 0; i < w->count; i++) {
    Rec_08a131c0 *r = &w->recs[i];
    Obj_08a131c0 *a = FUN_08a52b38(FUN_08a50fbc(), r->a);
    Obj_08a131c0 *b = FUN_08a52b38(FUN_08a50fbc(), r->b);
    FUN_08a12ec0(w, a, b, r->c, r->d, r->e);
  }
  if (w->recs)
    delete[] w->recs;
  w->recs = 0;
  w->count = 0;
  w->compacting = 0;
}
