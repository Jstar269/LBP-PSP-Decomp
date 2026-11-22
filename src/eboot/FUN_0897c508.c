/* LBPMATCH module=EBOOT.BIN start=0x0897c508 end=0x0897c5f0 symbol=FUN_0897c508 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Ctx { int pad; int f4; };
struct Owner { char pad[16]; void *child; };
extern struct Ctx *DAT_08be85fc;
extern void *FUN_08a9e838(int n);
extern void FUN_0896cc58(void *o);
extern void FUN_089d2804(void *o, int a, int b, int c);
extern void FUN_0896df60(void *a, void *b, int c);
extern void FUN_0896dbfc(void *o, int a, int b, int c);
extern void FUN_0896db58(void *o, int a, int b, int c);
extern void FUN_0896e1e8(void *o, int a, int b, int c);
extern void FUN_0896e114(void *o, int a, int b, int c);

void *FUN_0897c508(struct Owner *owner)
{
  void *o = FUN_08a9e838(104);
  int *c;
  int v;
  FUN_0896cc58(o);
  v = DAT_08be85fc->f4;
  c = (int *)FUN_08a9e838(192);
  FUN_089d2804(c, v, 0, 0);
  c[44] = -1;
  owner->child = c;
  FUN_0896df60(o, c, 1);
  FUN_0896dbfc(o, 1, 5, 3);
  FUN_0896db58(o, 1, 2, 0);
  FUN_0896e1e8(o, 1, 1, 0);
  FUN_0896e114(o, 1, 1, 0);
  return o;
}
