/* LBPMATCH module=EBOOT.BIN start=0x0894b31c end=0x0894b3e4 symbol=FUN_0894b31c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
inline void *operator new(unsigned, void *p) throw() { return p; }
struct Ev { void *vt; int kind; int arg; };
struct Sub { int a; int b; int c; };
struct Data { int f0, f4, f8; Sub sub; int f24, f28, f32; };
struct Vec {
    Ev **start;
    Ev **finish;
    Ev **eos;
};
extern "C" void *FUN_08a9e838(unsigned);
extern "C" void FUN_0894a948(Vec *v, Ev **pos, Ev *const *x);
extern "C" char DAT_08bb7798;
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
extern "C" Data *FUN_0894b31c(Q *q)
{
    Vec *v = &q->v;
    q->state = 2;
    Data *d = (Data *)FUN_08a9e838(36);
    Sub *sp = &d->sub;
    sp->c = 0;
    sp->b = 0;
    d->f24 = 1;
    d->f0 = 0;
    d->f4 = 0;
    d->f8 = 0;
    sp->a = 0;
    d->f28 = 0;
    d->f32 = 0;
    Ev *e = (Ev *)FUN_08a9e838(12);
    e->vt = &DAT_08bb7798;
    e->kind = 0;
    e->arg = (int)d;
    push_back(v, e);
    return d;
}
// NEAR 5 words: same queue-event family as the EXACT queue_event_* sources (Vec *v = &q->v at the top, Ev* in a stack temp for the const& push_back, inline push_back with f = v->finish loaded once). Only the register choice for the +12 sub-object pointer differs: retail 'addiu v1,v0,12; li v0,1; sw zero,8(v1); li a0,12; sw zero,4(v1)', ours reuses v0 (addiu v0,v0,12).
// near miss from lane api-lbp-a-20261121 (unreviewed)
