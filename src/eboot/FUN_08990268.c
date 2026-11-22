/* LBPMATCH module=EBOOT.BIN start=0x08990268 end=0x089902f8 symbol=FUN_08990268 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Self {
  const void *vt;
  char pad0[32];
  void *h36;
  void *h40;
  char pad1[76];
  char str[36];
  void *h156;
  void *h160;
};
extern char DAT_08bbfb80[];
extern void FUN_088050b8(void *p);
extern void FUN_08a9bff8(void *p);
extern void FUN_0897489c(void *p);
extern void FUN_08a9e0cc(void *p);

void FUN_08990268(struct Self *self)
{
  self->vt = DAT_08bbfb80;
  if (self->h160 != 0)
    FUN_088050b8(self->h160);
  if (self->h156 != 0)
    FUN_088050b8(self->h156);
  FUN_08a9bff8(self->str);
  if (self->h40 != 0)
    FUN_088050b8(self->h40);
  if (self->h36 != 0)
    FUN_088050b8(self->h36);
  FUN_0897489c(self);
  FUN_08a9e0cc(self);
}
