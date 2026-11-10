/* LBPMATCH module=EBOOT.BIN start=0x08883694 end=0x08883828 symbol=FUN_08883694 lang=c++ rodata=0x08bb0bf4 */
/* Hand-written reconstruction from the disassembly (lane lbp21o, Claude Opus 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08ad3088(void *m);
extern "C" void FUN_08ad2810(void *m);
extern "C" int DAT_08bef8f4;
struct RefObj {
    virtual void f0();
    virtual void destroy();
    int rc;
    void retain() { FUN_08ad3088(&DAT_08bef8f4); ++rc; FUN_08ad2810(&DAT_08bef8f4); }
    void release() {
        FUN_08ad3088(&DAT_08bef8f4);
        int n = --rc;
        FUN_08ad2810(&DAT_08bef8f4);
        if (n == 0) destroy();
    }
};
struct Ref {
    RefObj *p;
    Ref() : p(0) {}
    Ref(RefObj *q) : p(q) {}
    Ref(const Ref &o) {
        RefObj *q = o.p;
        p = q;
        if (q) q->retain();
    }
    ~Ref() {
        RefObj *q = p;
        if (q) q->release();
    }
    Ref &operator=(const Ref &o) {
        RefObj *q = o.p;
        if (q) q->retain();
        RefObj *old = p;
        p = q;
        if (old) old->release();
        return *this;
    }
};
extern "C" void *FUN_08a9e838(unsigned size);
extern "C" void FUN_08854dbc(void *o, const char *name, int a, int b);
extern "C" void FUN_0887cb4c(void *o, Ref r, int n);
extern "C" Ref DAT_08bedfb4;
extern "C" void *DAT_08bee02c;
inline RefObj *make_poster() {
    void *m = FUN_08a9e838(0x328);
    FUN_08854dbc(m, "para poster", 16, 64);
    return (RefObj *)m;
}
extern "C" void FUN_08883694()
{
    if (!DAT_08bedfb4.p)
        DAT_08bedfb4 = Ref(make_poster());
    FUN_0887cb4c(DAT_08bee02c, DAT_08bedfb4, 44);
}
