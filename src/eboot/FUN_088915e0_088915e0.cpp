/* LBPMATCH module=EBOOT.BIN start=0x088915e0 end=0x0889169c symbol=FUN_088915e0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" int DAT_08bef8f4;

extern "C" void FUN_0888b8c4(void *);
extern "C" void FUN_088912b8(void *, void *, void *, int, int, int);
extern "C" void FUN_0888b8cc(void *);
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);

struct Ref {
    void *vt;
    int rc;
    unsigned char pad[116 - 8];
    int f74;
};

struct SharedPtr {
    Ref *p;
    inline SharedPtr(const SharedPtr &o)
    {
        Ref *r = o.p;
        p = r;
        if (r != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            r->rc++;
            FUN_08ad2810(&DAT_08bef8f4);
        }
    }
};

struct Obj {
    unsigned char pad[44];
    int lock;
};

extern "C" SharedPtr FUN_088915e0(Obj *self, SharedPtr *p3)
{
    void *lock = &self->lock;
    int *key = &p3->p->f74;
    FUN_0888b8c4(lock);
    FUN_088912b8(self, key, p3, 1, 0, 2);
    FUN_0888b8cc(lock);
    return *p3;
}
