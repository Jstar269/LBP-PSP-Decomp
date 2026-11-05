/* LBPMATCH module=EBOOT.BIN start=0x08a0a334 end=0x08a0a3e8 symbol=FUN_08a0a334 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Body_08a0a334 { unsigned flags; };
struct Joint_08a0a334;
struct JointVt_08a0a334 { void *slots[19]; bool (*active)(Joint_08a0a334 *j); };
struct Joint_08a0a334 { JointVt_08a0a334 *vt; Body_08a0a334 *a; Body_08a0a334 *b; };
static inline bool Free_08a0a334(Body_08a0a334 *b)
{
  return (b->flags & 0xc00) && !(b->flags & 0x1000);
}
extern "C" bool FUN_08a0a334(Joint_08a0a334 *j)
{
  if (j->vt->active(j)) {
    if (Free_08a0a334(j->a) | Free_08a0a334(j->b))
      return true;
  }
  return false;
}
