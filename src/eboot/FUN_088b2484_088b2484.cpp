/* LBPMATCH module=EBOOT.BIN start=0x088b2484 end=0x088b2558 symbol=FUN_088b2484 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

inline void *operator new(unsigned, void *p) throw() { return p; }

struct Pool {
    int f0;
    void **arr;
    int cap;
    int count;
    int allocs;
};

extern "C" int DAT_08bef8f4;
extern "C" Pool DAT_08bee488;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void *FUN_08adb810(int, int, int, int);
extern "C" void FUN_089460d8(void *);

struct Item {
    unsigned char data[416];
    inline Item() { FUN_089460d8(this); }
    inline void *operator new(unsigned sz) { return FUN_08adb810(sz, 16, 0, 0); }
    inline void *operator new(unsigned, void *p) throw() { return p; }
};

struct Ptr {
    Item *p;
    inline Ptr(Item *x) : p(x) {}
    inline Ptr(const Ptr &o) : p(o.p) {}
};

extern "C" Ptr FUN_088b2484(void)
{
    FUN_08ad3088(&DAT_08bef8f4);
    FUN_08ad3088(&DAT_08bef8f4);
    Item *p;
    if (DAT_08bee488.count < DAT_08bee488.cap) {
        p = new (DAT_08bee488.arr[DAT_08bee488.count++]) Item();
    } else {
        DAT_08bee488.allocs++;
        p = new Item();
    }
    FUN_08ad2810(&DAT_08bef8f4);
    Ptr r(p);
    FUN_08ad2810(&DAT_08bef8f4);
    return r;
}
