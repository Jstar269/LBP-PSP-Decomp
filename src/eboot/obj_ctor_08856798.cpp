/* LBPMATCH module=EBOOT.BIN start=0x08856798 end=0x0885695c symbol=FUN_08856798 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp19o, Claude Opus 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0885d290(void *o, int a, int b, int c);
extern "C" void FUN_0885d244(void *o, int s);
inline void *operator new(unsigned, void *p) throw() { return p; }
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
extern "C" void FUN_08b9eb88(void *p, int c, unsigned n);
extern "C" const char *FUN_08887624();
extern "C" void FUN_08ba0868(char *buf, int n, const char *fmt, ...);
extern char DAT_08baf21c[];
extern char DAT_08baf238[];
extern void *DAT_08baf0f0[];
struct NodeBase { int color; NodeBase *parent; NodeBase *left; NodeBase *right; };
struct Tree {
    int cmp;
    NodeBase header;
    int count;
    void init() {
        FUN_08b9eb88(&header, 0, sizeof(header));
        header.color = 0;
        header.parent = 0;
        header.left = &header;
        header.right = &header;
        count = 0;
    }
};
inline RefObj *make_buffer() {
    void *m = FUN_08a9e838(0x34);
    FUN_08876c60(m, 0x400, 1, 0, 0);
    return (RefObj *)m;
}
struct Obj { void **vt; int pad[51]; Tree tree; };
extern "C" void FUN_0885cc04(Obj *o, const Ref &a, const Ref &b);
extern "C" void FUN_08856798(Obj *o, unsigned data_v, int exe_v)
{
    char url[256];
    FUN_0885d290(o, 4, 1, 5);
    o->vt = DAT_08baf0f0;
    o->tree.init();
    if (exe_v >= 0)
        FUN_08ba0868(url, 256, DAT_08baf21c, FUN_08887624(), data_v, exe_v);
    else
        FUN_08ba0868(url, 256, DAT_08baf238, FUN_08887624(), data_v);
    FUN_0885d244(o, (int)url);
    FUN_0885cc04(o, Ref(), Ref(make_buffer()));
}
