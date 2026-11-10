/* LBPMATCH module=EBOOT.BIN start=0x08866a7c end=0x08866c48 symbol=FUN_08866a7c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp21o, Claude Opus 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0885d244(void *o, const char *s);
extern "C" void *FUN_08a9e838(unsigned size);
extern "C" void FUN_08ad3088(void *m);
extern "C" void FUN_08ad2810(void *m);
extern "C" int DAT_08bef8f4;
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
extern "C" const char *FUN_08887630();
extern "C" void FUN_08ba0868(char *buf, int n, const char *fmt, ...);
extern char DAT_08baf7ac[];
extern char DAT_08baeee4[];
extern char DAT_08bb0008[];
extern char DAT_08baef80[];
struct Key { int type; unsigned id; };
inline const char *kind_name(int t)
{
    return t == 0 ? DAT_08baf7ac : t == 1 ? DAT_08baeee4 : 0;
}
inline RefObj *make_source(int *src) {
    void *m = FUN_08a9e838(0x18);
    FUN_08876bd8(m, src);
    return (RefObj *)m;
}
inline RefObj *make_buffer() {
    void *m = FUN_08a9e838(0x34);
    FUN_08876c60(m, 0x400, 1, 0, 0);
    return (RefObj *)m;
}
struct Obj;
extern "C" void FUN_0885cc04(Obj *o, const Ref &a, const Ref &b);
extern "C" void FUN_08866a7c(Obj *o, Key *key, int *src, int count)
{
    char url[256];
    if (count != 0)
        FUN_08ba0868(url, 256, DAT_08bb0008, FUN_08887630(), kind_name(key->type), key->id, count);
    else
        FUN_08ba0868(url, 256, DAT_08baef80, FUN_08887630(), kind_name(key->type), key->id);
    FUN_0885d244(o, url);
    FUN_0885cc04(o, Ref(make_source(src)), Ref(make_buffer()));
}
