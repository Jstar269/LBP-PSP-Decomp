/* LBPMATCH module=EBOOT.BIN start=0x0885b7f0 end=0x0885b86c symbol=FUN_0885b7f0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane fb-near1, Claude); names and types await human review. */
extern char DAT_08baf430, DAT_08bae6e0, DAT_08bedb94, DAT_08baec60;
extern void FUN_08a9e0c4(void *), FUN_08843f48(void *), FUN_0885c9f8(void *);

typedef struct Sub {
  void *vt;
  char pad[48];
  void *checked;
} Sub;

typedef struct Obj {
  void *vt0;
  char pad[212];
  Sub sub;
} Obj;

void FUN_0885b7f0(Obj *p)
{
  Sub *sub = &p->sub;

  p->vt0 = &DAT_08baf430;
  sub->vt = &DAT_08bae6e0;
  if (sub->checked != &DAT_08bedb94 && sub->checked)
    FUN_08a9e0c4(sub->checked);
  FUN_08843f48(sub);
  p->vt0 = &DAT_08baec60;
  FUN_0885c9f8(p);
}
