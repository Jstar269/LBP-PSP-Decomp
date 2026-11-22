/* LBPMATCH module=EBOOT.BIN start=0x08979d3c end=0x08979df8 symbol=FUN_08979d3c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj { char pad[6]; short rc; };
extern void FUN_0896b4a0(struct Obj **out, int a, int *x);
extern void FUN_08979514(struct Obj **out, int code, int *p);
extern void FUN_089797f0(void *out, struct Obj **a, struct Obj **b, int z);
extern void FUN_08970b6c(struct Obj *o);

void FUN_08979d3c(void *out, int a, int b, int kind)
{
  int x;
  struct Obj *r2;
  struct Obj *r1[2];
  int p[4];
  int code;
  p[0] = b;
  code = 19001;
  if (kind != 1) {
    if (kind == 2)
      code = 19002;
    else
      code = 19000;
  }
  FUN_0896b4a0(r1, a, &x);
  FUN_08979514(&r2, code, p);
  FUN_089797f0(out, r1, &r2, 0);
  if (r2 != 0)
    FUN_08970b6c(r2);
  if (r1[0] != 0)
    FUN_08970b6c(r1[0]);
}
