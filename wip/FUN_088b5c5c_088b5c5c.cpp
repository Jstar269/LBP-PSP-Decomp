/* LBPMATCH module=EBOOT.BIN start=0x088b5c5c end=0x088b5d00 symbol=FUN_088b5c5c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_088050b8(void *);

struct Item {
    int f0;
    int key;
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
    int key;
    RefPtr val;
    inline Pair(int k, const RefPtr &v) : key(k), val(v) {}
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

extern "C" RawRet FUN_0883d904(void *, const Pair *);

struct Tree {
    int dummy;
    inline Ret insert(const Pair &p)
    {
        return Ret(FUN_0883d904(this, &p));
    }
};

struct Obj {
    unsigned char pad[0x19c];
    Tree tree;
};

extern "C" bool FUN_088b5c5c(Obj *self, const RefPtr *arg)
{
    return self->tree.insert(Pair(arg->p->key, *arg)).ok;
}

// Remaining diffs (40/41 words; 17 differ at +0x5c): exact match up to FUN_0883d904 call; after call GCC stores v1 as a word (sw v1,4(sp)) instead of retail's lbu/sb copy of std::pair<iter,bool>::second.
// near miss from lane oc-lbp-20261121 (unreviewed)
