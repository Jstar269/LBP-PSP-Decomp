/* LBPMATCH module=EBOOT.BIN start=0x0895de34 end=0x0895df34 symbol=FUN_0895de34 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct VT { int a; void (*release)(void *); };
struct Obj { const struct VT *vt; int sub[15]; struct Obj *c40, *c44, *c48; void *c4c; void *c50; void *c54; };
extern char DAT_08bb8108[];
extern void *DAT_08bf03b8;
extern void FUN_0896f07c(void *a, int b, void *c);
extern void FUN_08a9e0c4(void *p);
extern void FUN_088050b8(void *p);
extern void FUN_08a9bff8(void *p);

void FUN_0895de34(struct Obj *self)
{
  self->vt = (const struct VT *)DAT_08bb8108;
  FUN_0896f07c(DAT_08bf03b8, 7, self->c48);
  FUN_0896f07c(DAT_08bf03b8, 7, self->c40);
  FUN_0896f07c(DAT_08bf03b8, 7, self->c44);
  if (self->c4c) {
    FUN_08a9e0c4(self->c4c);
    self->c4c = 0;
  }
  if (self->c48)
    self->c48->vt->release(self->c48);
  if (self->c44)
    self->c44->vt->release(self->c44);
  if (self->c40)
    self->c40->vt->release(self->c40);
  if (self->c54)
    FUN_088050b8(self->c54);
  if (self->c50)
    FUN_088050b8(self->c50);
  FUN_08a9bff8(&self->sub[1]);
}
