/* LBPMATCH module=EBOOT.BIN start=0x0890451c end=0x089045c0 symbol=FUN_0890451c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void FUN_089ee4b4(void *);
extern "C" void FUN_088050b8(void *);
extern "C" void FUN_08a9e0cc(void *);

struct RefPtr {
    void *ptr;

    inline void reset() {
        void *p = ptr;
        ptr = 0;
        if (p != 0) {
            FUN_088050b8(p);
        }
    }

    inline ~RefPtr() {
        if (ptr != 0) {
            FUN_088050b8(ptr);
        }
    }
};

struct Inner {
    int refcount;
    void *f04;
    RefPtr p1;
    RefPtr p2;

    inline ~Inner() {
        FUN_089ee4b4(f04);
        p1.reset();
        p2.reset();
    }
};

extern "C" void FUN_0890451c(Inner **param_1)
{
    Inner *p = *param_1;
    if (p != 0 && --p->refcount == 0) {
        p->~Inner();
        FUN_08a9e0cc(p);
    }
}
