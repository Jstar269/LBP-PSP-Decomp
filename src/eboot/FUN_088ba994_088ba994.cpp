/* LBPMATCH module=EBOOT.BIN start=0x088ba994 end=0x088baa24 symbol=FUN_088ba994 lang=c++ */
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

struct Msg {
    unsigned char pad[0x28];
    RefPtr item;
};

struct Obj {
    unsigned char pad[0x3c];
    RefPtr item;
};

extern "C" void FUN_088ba994(Obj *self, Msg **msg, int kind)
{
    if (kind == 3) {
        self->item = (*msg)->item;
    }
}
