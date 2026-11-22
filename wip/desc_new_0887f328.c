/* LBPMATCH module=EBOOT.BIN start=0x0887f328 end=0x0887f3d0 symbol=FUN_0887f328 lang=c99 rodata=0x08bb0a60 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern void *FUN_08a9e838(int);
extern void FUN_0885daa0(void *, const char *, void *);
struct Desc { int a, b, one, d, e, f, g, h, kind; };
struct Sub { int kind, a, b, c, d, e, f; };
struct Src { int p0; struct Sub sub; };
void *FUN_0887f328(struct Src *s)
{
    struct Desc d;
    struct Sub *p = &s->sub;
    void *o;
    d.a = p->a;
    d.b = p->b;
    d.kind = p->kind;
    d.d = p->d;
    d.e = 0;
    d.f = 0;
    d.g = 0;
    d.h = 0;
    d.one = 1;
    switch (d.kind) {
    case 7:
        d.g = p->e;
        break;
    case 5:
        d.f = p->c;
        d.e = 1;
        break;
    }
    o = FUN_08a9e838(444);
    FUN_0885daa0(o, "level browse", &d);
    return o;
}
// NEAR 2 words: lw order of the kind load (retail: lw v0,4(a1); lw v1,8(a1); lw a2,4(a0); lw a0,16(a1); ours has a2 before v1).
// switch with case 7 before case 5 gives retail's block layout; source order of the assignments and how kind is read (s->kind, p->kind, local) do not change the load order.
// near miss from lane api-lbp-a-20261121 (unreviewed)
