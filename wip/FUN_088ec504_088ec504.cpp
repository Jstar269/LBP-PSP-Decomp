/* LBPMATCH module=EBOOT.BIN start=0x088ec504 end=0x088ec5d0 symbol=FUN_088ec504 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Obj {
    int f0;
    unsigned int id;
    int ref;
    int pad[2];
    int lock;
};

extern "C" void FUN_08ad3088(int *);
extern "C" void FUN_08ad2810(int *);
extern "C" void FUN_088050b8(Obj *);

struct Ref {
    Obj *p;
    inline Ref(const Ref &o)
    {
        Obj *q = o.p;
        p = q;
        if (q != 0) {
            FUN_08ad3088(&q->lock);
            q->ref++;
            FUN_08ad2810(&q->lock);
        }
    }
    inline ~Ref()
    {
        if (p != 0) {
            FUN_088050b8(p);
        }
    }
};

struct Entry {
    unsigned int key;
    Ref val;
    inline Entry(unsigned int k, const Ref &v) : key(k), val(v) {}
};

struct InsRet {
    void *it;
    bool inserted;
};

struct Map {
    int f0;
    int header;
};

struct Ui {
    unsigned char pad[0xe0];
    Map map;
};

extern "C" void *FUN_088b1824(Map *, unsigned int *);
extern "C" InsRet FUN_0883d904(Map *, Entry *);

extern "C" void FUN_088ec504(Ui *param_1, const Ref *param_2)
{
    unsigned int key = param_2->p->id;
    if (FUN_088b1824(&param_1->map, &key) == &param_1->map.header) {
        Entry e(key, *param_2);
        InsRet r = FUN_0883d904(&param_1->map, &e);
        (void)r;
    }
}

// Near-miss note: Branch/regalloc diff on 49/51 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
