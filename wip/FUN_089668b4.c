/* LBPMATCH module=EBOOT.BIN start=0x089668b4 end=0x08966998 symbol=FUN_089668b4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj;
struct VT { int pad[13]; void (*f34)(struct Obj *, void *); };
struct Obj { const struct VT *vt; };
extern char DAT_08bb8860[];
extern unsigned FUN_08adbd60(char *s);
extern void *FUN_08a9e838(int n);
extern void FUN_089d5884(struct Obj *o, short a, short b, short c, short d, unsigned h);
extern void FUN_089d5bfc(struct Obj *o, int a, int b, int c);
extern void *FUN_08965c98(void);
extern void FUN_089667c0(struct Obj *o);
extern void FUN_08965dec(struct Obj *o);

struct Obj *FUN_089668b4(short a, short b, short c, short d, int e, int f)
{
  unsigned h = ~FUN_08adbd60(DAT_08bb8860);
  struct Obj *o = (struct Obj *)FUN_08a9e838(276);
  FUN_089d5884(o, a, b, c, d, h);
  FUN_089d5bfc(o, e, f, 0);
  o->vt->f34(o, FUN_08965c98());
  FUN_089667c0(o);
  FUN_08965dec(o);
  return o;
}
// NEAR: 2 words differ (57/57). Retail computes `nor s1,zero,v0` (the complemented hash) in the delay slot of the allocation call and keeps it in s1;
// ours copies the raw hash (`move s1,v0`) and applies the nor at the use (`nor t1,zero,s1`), the single-use temporary being forwarded into the call.
// Tried: ~ in the declaration, ~ at the use, whole call-in-arguments form (54 words, wrong order).
// near miss from lane api-lbp-b-20261121 (unreviewed)
