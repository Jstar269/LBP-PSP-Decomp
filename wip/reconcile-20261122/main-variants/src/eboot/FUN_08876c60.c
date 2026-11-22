/* LBPMATCH module=EBOOT.BIN start=0x08876c60 end=0x08876cf8 symbol=FUN_08876c60 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17a, Codex GPT-6); exact match verified; names and types await human review. */
typedef unsigned int u32;

struct Self {
  void *vtable;
  u32 field4;
  u32 field8;
  unsigned char flag_c;
  char pad0[3];
  char subobject[8];
  void *param2;
  u32 field1c;
  u32 field20;
  unsigned char flag24;
  unsigned char param3;
  char pad26[2];
  u32 param4;
  u32 param5;
  u32 field30;
};

extern char DAT_08bb0718[];
extern void FUN_0894a258();
extern void FUN_088768e4(void *);

void FUN_08876c60(struct Self *param_1, void *param_2, unsigned char param_3,
                  u32 param_4, u32 param_5)
{
  FUN_0894a258();
  param_1->field4 = 1;
  param_1->vtable = DAT_08bb0718;
  param_1->field8 = 0;
  param_1->flag_c = 0;
  FUN_088768e4(param_1->subobject);
  param_1->param2 = param_2;
  param_1->param3 = param_3;
  param_1->param4 = param_4;
  param_1->param5 = param_5;
  param_1->field30 = 0;
  param_1->field1c = 0;
  param_1->flag24 = 0;
  param_1->field20 = 0;
}
