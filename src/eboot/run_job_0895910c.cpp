/* LBPMATCH module=EBOOT.BIN start=0x0895910c end=0x089591f4 symbol=FUN_0895910c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Pair { int a, b; };
struct Sub {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6();
    virtual void call(int (*cb)(void *), int arg, int x, int y);
};
struct Obj {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5();
    virtual void run(Pair *p, int *q, int a3, int flag);
    char pad[4];
    int refs;
    char pad1[8];
    char lock[1];
    char pad2[147];
    Sub *sub;
};
struct Src { char pad[16]; Pair p; char pad1[16]; int f40; int f44; int pad2; int f52; };
struct Holder { Src *s; };
extern "C" void *DAT_08be84a8;
extern "C" Obj *FUN_08aaaa80(void *, void *);
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_088050b8(Obj *);
extern "C" int FUN_0895874c(void *);
extern "C" void FUN_0895910c(Holder *h, int arg)
{
    Obj *o = FUN_08aaaa80(DAT_08be84a8, &h->s->p);
    if (o) {
        FUN_08ad3088(o->lock);
        o->refs = o->refs + 1;
        FUN_08ad2810(o->lock);
    }
    Src *s = h->s;
    Pair pr;
    int v;
    v = s->f44;
    pr.a = s->p.a;
    pr.b = s->p.b;
    o->run(&pr, &v, s->f40, s->f52 == 0);
    o->sub->call(FUN_0895874c, arg, 0, 1);
    FUN_088050b8(o);
}
