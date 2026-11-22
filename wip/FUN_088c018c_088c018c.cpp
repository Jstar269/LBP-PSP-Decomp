/* LBPMATCH module=EBOOT.BIN start=0x088c018c end=0x088c0258 symbol=FUN_088c018c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_088050b8(void *);

struct Item {
    int f0;
    unsigned int key;
    int rc;
    int f0c;
    int f10;
    int lock;
};

struct RefPtr {
    Item *p;
    inline RefPtr(const RefPtr &o)
    {
        Item *r = o.p;
        p = r;
        if (r != 0) {
            FUN_08ad3088(&r->lock);
            ++r->rc;
            FUN_08ad2810(&r->lock);
        }
    }
    inline ~RefPtr()
    {
        if (p != 0) {
            FUN_088050b8(p);
        }
    }
};

struct Pair {
    unsigned int key;
    RefPtr val;
    inline Pair(unsigned int k, const RefPtr &v) : key(k), val(v) {}
};

struct RawRet {
    void *it;
    bool ok;
};

struct Ret {
    void *it;
    bool ok;
    inline Ret(const RawRet &r) : it(r.it), ok(r.ok) {}
};

struct Tree {
    int pad;
    int header[4];
    int count;
};

extern "C" void *FUN_088b1824(Tree *, const unsigned int *);
extern "C" RawRet FUN_0883d904(Tree *, const Pair *);

struct Obj {
    int pad[6];
    Tree tree;
};

extern "C" void FUN_088c018c(Obj *self, const RefPtr *arg)
{
    unsigned int k = arg->p->key;
    if (FUN_088b1824(&self->tree, &k) == &self->tree.header) {
        Pair pair(k, *arg);
        Ret r(FUN_0883d904(&self->tree, &pair));
        (void)r;
    }
}

// Remaining diffs (49/51 words; 37 differ): same std::pair<iter,bool> return-value copy difference as FUN_088b5c5c, plus stack layout.
// near miss from lane oc-lbp-20261121 (unreviewed)
