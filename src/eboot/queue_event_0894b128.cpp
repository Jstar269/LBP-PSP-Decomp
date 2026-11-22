/* LBPMATCH module=EBOOT.BIN start=0x0894b128 end=0x0894b1f4 symbol=FUN_0894b128 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
inline void *operator new(unsigned, void *p) throw() { return p; }
struct Ev { void *vt; int kind; int arg; };
struct Ev4 : Ev { int f12; int f16; int f20; int f24; };
struct Vec {
    Ev **start;
    Ev **finish;
    Ev **eos;
};
extern "C" void *FUN_08a9e838(unsigned);
extern "C" void FUN_0894a948(Vec *v, Ev **pos, Ev *const *x);
extern "C" char DAT_08bb7818;
inline void push_back(Vec *v, Ev *const &x)
{
    Ev **f = v->finish;
    if (f != v->eos) {
        new (f) Ev *(x);
        v->finish = f + 1;
    } else
        FUN_0894a948(v, f, &x);
}
struct Q { int state; int pad[2]; Vec v; };
extern "C" Ev4 *FUN_0894b128(Q *q, int a, int b, int c, int d)
{
    Ev4 *e = (Ev4 *)FUN_08a9e838(28);
    e->kind = 4;
    e->vt = &DAT_08bb7818;
    e->arg = a;
    e->f16 = b;
    e->f20 = c;
    e->f24 = d;
    e->f12 = 0;
    push_back(&q->v, e);
    return e;
}
