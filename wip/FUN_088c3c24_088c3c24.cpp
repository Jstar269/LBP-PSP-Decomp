/* LBPMATCH module=EBOOT.BIN start=0x088c3c24 end=0x088c3d20 symbol=FUN_088c3c24 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void *DAT_08be84a8;
extern "C" void FUN_08a9c0b4(void *, void *);
extern "C" void FUN_08aaa9a0(void *);
extern "C" unsigned int FUN_08a9d790(void *);
extern "C" void FUN_08a9d390(void *, unsigned int);
extern "C" void FUN_08a9bff8(void *);
extern "C" void *FUN_08aaa70c(void *, unsigned int *);
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_088050b8(void *);

struct Str {
    int data[6];
    inline Str(void *src) { FUN_08a9c0b4(this, src); }
    inline Str(unsigned int id) { FUN_08a9d390(this, id); }
    inline ~Str() { FUN_08a9bff8(this); }
};

struct Item {
    void (**vt)(Item *, void *);
    int f4;
    int rc;
    int f0c;
    int f10;
    int lock;
};

struct RefPtr {
    Item *p;
    inline RefPtr(Item *x)
    {
        p = x;
        if (x != 0) {
            FUN_08ad3088(&x->lock);
            ++x->rc;
            FUN_08ad2810(&x->lock);
        }
    }
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

struct Msg {
    int pad[4];
    int str;
};

extern "C" void FUN_088c3c24(Msg **param_1, void *param_2)
{
    Str s(&(*param_1)->str);
    FUN_08aaa9a0(&s);
    unsigned int id = FUN_08a9d790(&s);
    {
        Str tmp(id);
    }
    RefPtr r = RefPtr((Item *)FUN_08aaa70c(DAT_08be84a8, &id));
    r.p->vt[5](r.p, param_2);
}

// Remaining diffs (54/63 words; 28 differ): retail constructs and copies a temporary RefPtr (double refcount inc + dec) before the vtable call, whereas GCC elides the copy here.
// near miss from lane oc-lbp-20261121 (unreviewed)
