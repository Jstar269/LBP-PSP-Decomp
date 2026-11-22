/* LBPMATCH module=EBOOT.BIN start=0x0897265c end=0x089726ec symbol=FUN_0897265c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj { char pad[668]; int idx; };
extern short *DAT_08bdb90c[];
extern void FUN_08971a00(void *out, struct Obj *o, short *s, int a, int b);
extern void FUN_089724f0(void *out, struct Obj *o, int id, int a, int b);
extern void FUN_089725a4(void *out, struct Obj *o, int id, int a, int b);
extern void FUN_088ec728(void *out, int a);

void *FUN_0897265c(void *out, struct Obj *o, int id, int a, int b)
{
  if (id == 900) {
    FUN_08971a00(out, o, DAT_08bdb90c[o->idx], a, b);
  } else if (id < 614) {
    FUN_089724f0(out, o, id, a, b);
  } else if ((unsigned)(id - 615) < 285) {
    FUN_089725a4(out, o, id, a, b);
  } else {
    FUN_088ec728(out, 0);
  }
  return out;
}
