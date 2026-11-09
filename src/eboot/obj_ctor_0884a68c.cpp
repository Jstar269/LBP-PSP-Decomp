/* LBPMATCH module=EBOOT.BIN start=0x0884a68c end=0x0884a828 symbol=FUN_0884a68c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp19o, Claude Opus 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0885d290(void *o, int a, int b, int c);
extern "C" void FUN_0885d244(void *o, int s);
extern "C" void *FUN_08a9e838(unsigned size);
extern "C" void FUN_08ad3088(void *m);
extern "C" void FUN_08ad2810(void *m);
extern "C" int DAT_08bef8f4;
extern void *DAT_08baec30[];
struct RefObj {
    virtual void f0();
    virtual void destroy();
    int rc;
};
struct Ref {
    RefObj *p;
    Ref() : p(0) {}
    Ref(RefObj *q) : p(q) {}
    ~Ref() {
        RefObj *q = p;
        if (q) {
            FUN_08ad3088(&DAT_08bef8f4);
            int n = --q->rc;
            FUN_08ad2810(&DAT_08bef8f4);
            if (n == 0) q->destroy();
        }
    }
};
extern "C" void FUN_08876c60(void *o, int size, unsigned char mode, int a, int b);
extern "C" void FUN_08876bd8(void *o, int *src);
struct Params { int f0; int f4; int f8; int fc; unsigned char f10; char pad[3]; int f14; };
struct Obj { void **vt; int pad[51]; Params params; };
extern "C" void FUN_0885cc04(Obj *o, const Ref &a, const Ref &b);
extern "C" void FUN_0884a68c(Obj *o, int *src, Params *p)
{
    FUN_0885d290(o, p->f0, p->f8, p->fc);
    o->params = *p;
    o->vt = DAT_08baec30;
    FUN_0885d244(o, p->f4);
    void *m = FUN_08a9e838(0x34);
    FUN_08876c60(m, 0x400, p->f10, 0, 0);
    Ref buf((RefObj *)m);
    if (*src != 0) {
        void *s = FUN_08a9e838(0x18);
        FUN_08876bd8(s, src);
        FUN_0885cc04(o, Ref((RefObj *)s), buf);
    } else {
        FUN_0885cc04(o, Ref(), buf);
    }
}
