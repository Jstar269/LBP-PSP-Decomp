/* LBPMATCH module=EBOOT.BIN start=0x0894c0c4 end=0x0894c154 symbol=FUN_0894c0c4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Ev { void *vt; int kind; struct Q *sub; int type() const { return kind; } };
struct Vec {
    Ev **start; Ev **finish; Ev **eos;
    Ev **begin() { return start; }
    Ev **end() { return finish; }
};
struct Q { int state; int pad[2]; Vec v; };
extern "C" void FUN_0894bec8(Ev *, int, int);
extern "C" void FUN_0894c0c4(Q *q, int a, int b)
{
    for (Ev **p = q->v.begin(); p != q->v.end(); ++p) {
        Ev *e = *p;
        if (e->type() == 4)
            FUN_0894bec8(e, a, b);
        else if (e->type() == 0)
            FUN_0894c0c4(e->sub, a, b);
    }
}
