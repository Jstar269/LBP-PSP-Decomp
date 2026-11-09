/* LBPMATCH module=EBOOT.BIN start=0x088456e4 end=0x088457bc symbol=FUN_088456e4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp19o, Claude Opus 5.5, from the lbp-decomp-b lane draft); exact match verified; names and types await human review. */
extern "C" void *FUN_08a9e838(int n);
extern "C" int FUN_08ba0e8c(const char *s);
extern "C" void FUN_08842ef4(void *p, const char *s, int n);
extern char DAT_08bedb94[];
extern char DAT_08bd6488[];
extern void *DAT_08bae290[];

struct Obj_088456e4 {
  void **vtable;
  int field_04;
  int field_08;
  int field_0c;
  int field_10;
  int field_14;
  int field_18;
  int field_1c;
  char *field_20;
  int field_24;
  int field_28;
  char field_2c;
};

extern "C" Obj_088456e4 *FUN_088456e4(Obj_088456e4 *src)
{
  Obj_088456e4 *o = (Obj_088456e4 *)FUN_08a9e838(48);
  o->field_08 = -1;
  o->field_04 = -1;
  o->field_20 = DAT_08bedb94;
  o->field_14 = 4;
  o->vtable = DAT_08bae290;
  o->field_0c = 0;
  o->field_10 = 0;
  o->field_18 = 0;
  o->field_1c = 0;
  o->field_24 = 0;
  o->field_28 = 0;
  FUN_08842ef4(&o->field_20, DAT_08bd6488, 0);
  o->field_2c = 0;
  char *s = src->field_20 + 8;
  FUN_08842ef4(&o->field_20, s, FUN_08ba0e8c(s));
  o->field_0c = src->field_0c;
  o->field_2c = src->field_2c;
  return o;
}
