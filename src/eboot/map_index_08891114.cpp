/* LBPMATCH module=EBOOT.BIN start=0x08891114 end=0x088912b8 symbol=FUN_08891114 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp21o, Claude Opus 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08ad3088(void *m);
extern "C" void FUN_08ad2810(void *m);
extern "C" int DAT_08bef8f4;
struct RefObj {
    virtual void f0();
    virtual void destroy();
    int rc;
};
struct Ref;
extern "C" void FUN_08890574(Ref *self, const Ref *o);
struct Ref {
    RefObj *p;
    Ref() : p(0) {}
    ~Ref() {
        RefObj *q = p;
        if (q) {
            FUN_08ad3088(&DAT_08bef8f4);
            int n = --q->rc;
            FUN_08ad2810(&DAT_08bef8f4);
            if (n == 0) q->destroy();
        }
    }
    Ref &operator=(const Ref &o) { FUN_08890574(this, &o); return *this; }
};
struct Value_08891114 {
    int a;
    bool b;
    int c;
    Ref r;
    Value_08891114() { r = Ref(); a = 0; c = 0; b = false; }
    Value_08891114(const Value_08891114 &o);
};
struct Key {
    int a;
    unsigned b;
    bool operator<(const Key &o) const { return a < o.a || (a == o.a && b < o.b); }
};
struct Pair {
    Key key;
    Value_08891114 value;
    Pair(const Key &k, const Value_08891114 &v) : key(k), value(v) {}
};
struct MapNodeBase { int color; MapNodeBase *parent; MapNodeBase *left; MapNodeBase *right; };
struct MapNode : MapNodeBase { Key key; Value_08891114 value; };
struct Map {
    int compare;
    MapNodeBase header;
    unsigned count;
};
extern "C" MapNode *FUN_088900c0(Map *m, const Key *k);
extern "C" MapNode *FUN_08890ebc(Map *m, MapNode *hint, const Pair *v);
extern "C" Value_08891114 *FUN_08891114(Map *m, const Key *k)
{
    MapNode *i = FUN_088900c0(m, k);
    if (i == &m->header || *k < i->key)
        i = FUN_08890ebc(m, i, &Pair(*k, Value_08891114()));
    return &i->value;
}
