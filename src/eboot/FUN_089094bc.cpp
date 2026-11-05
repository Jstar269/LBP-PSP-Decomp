/* LBPMATCH module=EBOOT.BIN start=0x089094bc end=0x08909534 symbol=FUN_089094bc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0898e3f4(void *p);
extern "C" void FUN_08921a38(void *p);
extern "C" void FUN_08a9bff8(void *p);
extern "C" void FUN_0899e524(void *p);
extern void *DAT_08bb5e28[], *DAT_08bad898[], *DAT_08bae0b8[];
struct Obj { void **vt; int pad[9]; void **vt40; int pad2[35]; void **vt184; };
extern "C" void FUN_089094bc(Obj *o)
{
  o->vt = DAT_08bb5e28;
  FUN_0898e3f4((char *)o + 200);
  o->vt184 = DAT_08bad898;
  FUN_08921a38((char *)o + 148);
  FUN_08921a38((char *)o + 120);
  FUN_08a9bff8((char *)o + 96);
  FUN_08a9bff8((char *)o + 72);
  o->vt40 = DAT_08bae0b8;
  FUN_08a9bff8((char *)o + 16);
  FUN_0899e524(o);
}
