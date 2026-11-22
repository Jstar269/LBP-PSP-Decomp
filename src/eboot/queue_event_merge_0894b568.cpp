/* LBPMATCH module=EBOOT.BIN start=0x0894b568 end=0x0894b644 symbol=FUN_0894b568 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
inline void *operator new(unsigned, void *p) throw() { return p; }
struct Ev { void *vt; int kind; int arg; };
struct Ev32 : Ev { int pad[5]; };
extern "C" void FUN_0894b530(Ev32 *);
extern "C" void FUN_0894af44(Ev32 *, int, int, int);
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
extern "C" void FUN_0894b568(Q *q, int a, int b, int c)
{
    Ev32 *e;
    Ev *back;
    Vec *v = &q->v;
    q->state = 1;
    if (q->v.start == v->finish || (back = q->v.finish[-1])->kind != 1) {
        e = (Ev32 *)FUN_08a9e838(32);
        FUN_0894b530(e);
        push_back(v, e);
    } else
        e = (Ev32 *)back;
    FUN_0894af44(e, a, b, c);
}
