/* LBPMATCH module=EBOOT.BIN start=0x088770b8 end=0x08877148 symbol=FUN_088770b8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17a, Codex GPT-6); exact match verified; names and types await human review. */
typedef unsigned int u32;
typedef unsigned short u16;

struct Self {
  void *vtable;
  int initialized;
  u16 *buffer;
  int length;
};

extern char DAT_08bb0738[];
extern void FUN_0894a258();
extern u32 FUN_08ba0e8c(const char *);
extern void *FUN_08a9e824(u32);
extern void FUN_08889048(void *, const unsigned char *, u32, u32 *);

void FUN_088770b8(struct Self *param_1, unsigned char *param_2)
{
  u32 local_30[4];
  u32 size;
  u16 *buffer;

  FUN_0894a258();
  param_1->initialized = 1;
  param_1->vtable = DAT_08bb0738;
  size = FUN_08ba0e8c((const char *)param_2) + 1;
  buffer = (u16 *)FUN_08a9e824(size * 2);
  FUN_08889048(buffer, param_2, size, local_30);
  param_1->buffer = buffer;
  param_1->length = local_30[0] - 1;
}
