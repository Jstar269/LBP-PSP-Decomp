/* LBPMATCH module=EBOOT.BIN start=0x08949a70 end=0x08949ad4 symbol=FUN_08949a70 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp19o, Claude Opus 5.5); exact match verified; names and types await human review. */
extern "C" void *memset(void *p, int v, unsigned int n);
extern "C" void FUN_0894a258(void *p);
extern void *DAT_08bb76d0[], *DAT_08bb7700[];

struct Obj_08949a70 {
  void **vtable;
  int field_04;
  int field_08;
  int field_0c;
  int field_10;
  int field_14;
  char field_18[20];
};

extern "C" void FUN_08949a70(Obj_08949a70 *o)
{
  FUN_0894a258(o);
  o->field_04 = 1;
  o->vtable = DAT_08bb76d0;
  o->field_08 = 0;
  o->field_0c = 0;
  o->field_10 = 0;
  o->field_14 = 0;
  memset(o->field_18, 0, sizeof o->field_18);
  o->vtable = DAT_08bb7700;
}
