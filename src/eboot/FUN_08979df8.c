/* LBPMATCH module=EBOOT.BIN start=0x08979df8 end=0x08979e88 symbol=FUN_08979df8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj { char pad[6]; short rc; };
extern void FUN_0896b4a0(struct Obj **out, int a, char *x);
extern void FUN_089795b4(struct Obj **out, int code, char *y);
extern void FUN_089797f0(void *out, struct Obj **a, struct Obj **b, int z);
extern void FUN_08970b6c(struct Obj *o);

void FUN_08979df8(void *out, int a)
{
  char y;
  char x;
  struct Obj *r2;
  struct Obj *r1[2];
  FUN_0896b4a0(r1, a, &x);
  FUN_089795b4(&r2, 18000, &y);
  FUN_089797f0(out, r1, &r2, 0);
  if (r2 != 0)
    FUN_08970b6c(r2);
  if (r1[0] != 0)
    FUN_08970b6c(r1[0]);
}
