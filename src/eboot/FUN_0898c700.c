/* LBPMATCH module=EBOOT.BIN start=0x0898c700 end=0x0898c794 symbol=FUN_0898c700 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Str { char b[24]; void *h24; void *h28; };
struct Self { const void *vt; char pad[12]; struct Str s; void *h48; int pad1[1]; char sub[1]; };
extern char DAT_08bbfae8[];
extern char DAT_08babc90[];
extern void FUN_0890451c(void *p);
extern void FUN_088050b8(void *p);
extern void FUN_08a9bff8(void *p);
extern void FUN_08a9e0cc(void *p);

void FUN_0898c700(struct Self *self)
{
  struct Str *sp = &self->s;
  self->vt = DAT_08bbfae8;
  FUN_0890451c(self->sub);
  if (self->h48 != 0)
    FUN_088050b8(self->h48);
  if (sp->h28 != 0)
    FUN_088050b8(sp->h28);
  if (sp->h24 != 0)
    FUN_088050b8(sp->h24);
  FUN_08a9bff8(sp);
  self->vt = DAT_08babc90;
  FUN_08a9e0cc(self);
}
