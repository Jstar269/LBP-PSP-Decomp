/* LBPMATCH module=EBOOT.BIN start=0x088ce628 end=0x088ce6bc symbol=FUN_088ce628 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_088050b8(void *);

struct Item {
    int f0;
    int f4;
    int rc;
    int f0c;
    int f10;
    int lock;
};

struct RefPtr {
    Item *p;
    inline RefPtr &operator=(const RefPtr &o)
    {
        Item *n = o.p;
        if (n != 0) {
            FUN_08ad3088(&n->lock);
            ++n->rc;
            FUN_08ad2810(&n->lock);
        }
        Item *old = p;
        p = n;
        if (old != 0) {
            FUN_088050b8(old);
        }
        return *this;
    }
};

struct Obj {
    RefPtr arr[64];
    int count;
    unsigned char flag;
};

extern "C" void FUN_088ce628(Obj *self, const RefPtr *param_2)
{
    self->arr[self->count++] = *param_2;
    self->flag = 0;
}
