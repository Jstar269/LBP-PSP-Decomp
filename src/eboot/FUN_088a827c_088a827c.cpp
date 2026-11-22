/* LBPMATCH module=EBOOT.BIN start=0x088a827c end=0x088a8318 symbol=FUN_088a827c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void FUN_089ee4b4(int);
extern "C" void FUN_088050b8(void *);
extern "C" void FUN_08a9e0cc(void *);

struct Ptr {
    void *p;
    inline void reset()
    {
        void *t = p;
        p = 0;
        if (t != 0) {
            FUN_088050b8(t);
        }
    }
    inline ~Ptr()
    {
        if (p != 0) {
            FUN_088050b8(p);
        }
    }
};

struct Obj {
    int rc;
    int f4;
    Ptr p1;
    Ptr p2;
    inline ~Obj()
    {
        FUN_089ee4b4(f4);
        p1.reset();
        p2.reset();
    }
};

extern "C" void FUN_088a827c(Obj *self)
{
    if (--self->rc == 0) {
        self->~Obj();
        FUN_08a9e0cc(self);
    }
}
