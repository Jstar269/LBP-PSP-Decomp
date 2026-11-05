/* LBPMATCH module=EBOOT.BIN start=0x08a0bc24 end=0x08a0bc64 symbol=FUN_08a0bc24 lang=c++ cc=gcc404xs-O2 asm=vfpu */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review; matching idiom: a dummy "r"(f) input on the lv.q asm (the only exact form found: it orders the vector load after the frame pointer load, as retail does). */
/* Original inline VFPU asm: vector loads, stores and moves the C compiler cannot express. */
struct Frame_08a0bc24 { char pad[48]; float pos[4]; };
struct Joint_08a0bc24 { int pad0; Frame_08a0bc24 *a; char pad[184]; float v[4]; char pad2[16]; int e0; int e4; };
extern "C" void FUN_08a0a6bc(Joint_08a0bc24 *j);
extern "C" void FUN_089f9090(float *pos);
extern "C" void FUN_08a0bc24(Joint_08a0bc24 *j)
{
  FUN_08a0a6bc(j);
  Frame_08a0bc24 *f = j->a;
  __asm__ volatile("lv.q C000, %0" : : "m"(j->v[0]), "r"(f));
  FUN_089f9090(f->pos);
  __asm__ volatile("sv.q C000, %0" : "=m"(j->v[0]));
  j->e0 = 0;
  j->e4 = 0;
}
