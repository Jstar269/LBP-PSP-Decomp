/* LBPMATCH module=EBOOT.BIN start=0x08a4c4fc end=0x08a4c55c symbol=FUN_08a4c4fc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08a9df6c(void *p);
extern void *DAT_08bc5ce0[], *DAT_08bc5cf0[];
struct Obj { void **vt; int pad[8]; void *f36; int pad1[2]; void *f48; int pad2[2]; void *f60; int pad3[6]; void *f88; int pad4[1]; void *f96; };
extern "C" void FUN_08a4c4fc(Obj *o)
{
  o->vt = DAT_08bc5ce0;
  FUN_08a9df6c(o->f36);
  FUN_08a9df6c(o->f48);
  FUN_08a9df6c(o->f60);
  FUN_08a9df6c(o->f88);
  FUN_08a9df6c(o->f96);
  o->vt = DAT_08bc5cf0;
}
