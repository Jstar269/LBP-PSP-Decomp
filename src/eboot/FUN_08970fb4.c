/* LBPMATCH module=EBOOT.BIN start=0x08970fb4 end=0x0897107c symbol=FUN_08970fb4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Self { char pad[684]; void *a; void *b; void *c; void *d; };
extern char DAT_08bba6f8[];
extern char DAT_08bba70c[];
extern char DAT_08bba720[];
extern char DAT_08bba738[];
extern void *FUN_08a9e838(int n);
extern void FUN_08965b48(void *o, char *name, int a, int b);

void FUN_08970fb4(struct Self *self)
{
  void *o;
  o = FUN_08a9e838(8);
  FUN_08965b48(o, DAT_08bba6f8, 1, 29);
  self->a = o;
  o = FUN_08a9e838(8);
  FUN_08965b48(o, DAT_08bba70c, 6, 16);
  self->b = o;
  o = FUN_08a9e838(8);
  FUN_08965b48(o, DAT_08bba720, 0, 8);
  self->c = o;
  o = FUN_08a9e838(8);
  FUN_08965b48(o, DAT_08bba738, 0, 7);
  self->d = o;
}
