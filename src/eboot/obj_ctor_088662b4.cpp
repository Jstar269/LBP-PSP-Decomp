/* LBPMATCH module=EBOOT.BIN start=0x088662b4 end=0x08866470 symbol=FUN_088662b4 lang=c++ rodata=0x08baff58 */
/* Hand-written reconstruction from the disassembly (lane lbp19o, Claude Opus 5.5); exact match verified; names and types await human review. */
inline void *operator new(unsigned, void *p) throw() { return p; }
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
extern "C" void FUN_08879430(void *o, int a, void (*fn)(), int n);
extern "C" void FUN_088658d8();
extern "C" void FUN_0885d290(void *o, int a, int b, int c);
extern "C" void FUN_0885d244(void *o, const char *s);
extern "C" const char *FUN_08887624();
extern "C" void FUN_08ba0868(char *buf, int n, const char *fmt, ...);
extern "C" void FUN_08865ec0(void *o);
extern "C" void FUN_08845538(void *o);
extern "C" void *FUN_08a9e838(unsigned size);
extern "C" void FUN_0886b250(void *o, const char *name);
extern void *DAT_08bafea8[];
extern void *DAT_08bafee0[];
extern void *DAT_08bafe48[];
extern char DAT_08baff34[];
struct Sub {
    void **vt;
    int pad[51];
    void init() {
        char url[256];
        FUN_0885d290(this, 2, 0, 5);
        vt = DAT_08bafe48;
        FUN_08ba0868(url, 256, DAT_08baff34, FUN_08887624());
        FUN_0885d244(this, url);
    }
};
struct Obj {
    void **vt0;
    int pad4[2];
    void **vt3;
    int pad10[46];
    int fc8;
    Sub sub;
    char sub2[0x274 - 0x19c];
    Ref f274;
    Ref f278;
    char f27c[4];
};
inline RefObj *make_upload() {
    void *m = FUN_08a9e838(0x1a0);
    FUN_0886b250(m, "avatar upload");
    return (RefObj *)m;
}
extern "C" void FUN_088662b4(Obj *o, int a, Ref *src)
{
    FUN_08879430(o, a, FUN_088658d8, 3);
    o->vt0 = DAT_08bafea8;
    o->vt3 = DAT_08bafee0;
    o->fc8 = 0;
    o->sub.init();
    FUN_08865ec0(o->sub2);
    RefObj *q = src->p;
    o->f274.p = 0;
    o->f278.p = q;
    if (q) q->retain();
    FUN_08845538(o->f27c);
    o->f274 = Ref(make_upload());
}
