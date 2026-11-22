/* LBPMATCH module=EBOOT.BIN start=0x0898cbfc end=0x0898cc8c symbol=FUN_0898cbfc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Str { char b[24]; void *h24; void *h28; };
struct Self { const void *vt; char pad[20]; struct Str s; void *h56; int pad1; char sub[1]; };
extern char DAT_08bbfac0[];
extern char DAT_08bbf6c0[];
extern void FUN_0890451c(void *p);
extern void FUN_088050b8(void *p);
extern void FUN_08a9bff8(void *p);

void FUN_0898cbfc(struct Self *self)
{
  struct Str *sp = &self->s;
  self->vt = DAT_08bbfac0;
  FUN_0890451c(self->sub);
  if (self->h56 != 0)
    FUN_088050b8(self->h56);
  if (sp->h28 != 0)
    FUN_088050b8(sp->h28);
  if (sp->h24 != 0)
    FUN_088050b8(sp->h24);
  FUN_08a9bff8(sp);
  self->vt = DAT_08bbf6c0;
}
