/* LBPMATCH module=EBOOT.BIN start=0x0894b3e4 end=0x0894b474 symbol=FUN_0894b3e4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
inline void *operator new(unsigned, void *p) throw() { return p; }
struct Ev { void *vt; int kind; int arg; };
struct Vec {
    Ev **start;
    Ev **finish;
    Ev **eos;
};
extern "C" void *FUN_08a9e838(unsigned);
extern "C" void FUN_0894a948(Vec *v, Ev **pos, Ev *const *x);
extern "C" char DAT_08bb77d8;
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
extern "C" void FUN_0894b3e4(Q *q, int arg)
{
    Vec *v = &q->v;
    q->state = 1;
    Ev *e = (Ev *)FUN_08a9e838(12);
    e->kind = 2;
    e->vt = &DAT_08bb77d8;
    e->arg = arg;
    push_back(v, e);
}
