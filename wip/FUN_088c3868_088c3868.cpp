/* LBPMATCH module=EBOOT.BIN start=0x088c3868 end=0x088c395c symbol=FUN_088c3868 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void FUN_089f1a64(int);
extern "C" void FUN_089f19c0(int);
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_088050b8(void *);
extern "C" void FUN_088c1634(void);

struct Sub {
    void (**vt)(Sub *, void (*)(void), void *, int, int);
};

struct Item {
    int f0;
    int f4;
    int rc;
    int kind;
    int f10;
    int lock;
    unsigned char pad[0xa8 - 0x18];
    Sub *sub;
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

struct Msg {
    unsigned char pad[0x3c];
    int f3c;
};

struct Target {
    void (**vt)(Target *, const RefPtr *, void *, int);
};

extern "C" void FUN_088c3868(const RefPtr *param_1, void *param_2, Msg **param_3, Target *param_4)
{
    FUN_089f1a64((*param_3)->f3c);
    if (param_1->p->kind == 10) {
        RefPtr rp(*param_1);
        rp.p->sub->vt[7](rp.p->sub, FUN_088c1634, param_4, 0, 1);
    }
    param_4->vt[13](param_4, param_1, param_2, 2);
    FUN_089f19c0((*param_3)->f3c);
}

// Remaining diffs (57/61 words; 46 differ): retail saves an extra s-reg and keeps RefPtr check around ~RefPtr differently (4 words shorter).
// near miss from lane oc-lbp-20261121 (unreviewed)
