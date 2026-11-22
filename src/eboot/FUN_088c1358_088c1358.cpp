/* LBPMATCH module=EBOOT.BIN start=0x088c1358 end=0x088c142c symbol=FUN_088c1358 lang=c++ */
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
extern "C" Pool DAT_08bee614;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void *FUN_08adb810(int, int, int, int);
extern "C" void FUN_089323e0(void *);

struct Item {
    unsigned char data[0x48];
    inline Item() { FUN_089323e0(this); }
    inline void *operator new(unsigned sz) { return FUN_08adb810(sz, 16, 0, 0); }
    inline void *operator new(unsigned, void *p) throw() { return p; }
};

struct Ptr {
    Item *p;
    inline Ptr(Item *x) : p(x) {}
    inline Ptr(const Ptr &o) : p(o.p) {}
};

extern "C" Ptr FUN_088c1358(void)
{
    FUN_08ad3088(&DAT_08bef8f4);
    FUN_08ad3088(&DAT_08bef8f4);
    Item *p;
    if (DAT_08bee614.count < DAT_08bee614.cap) {
        p = new (DAT_08bee614.arr[DAT_08bee614.count++]) Item();
    } else {
        DAT_08bee614.allocs++;
        p = new Item();
    }
    FUN_08ad2810(&DAT_08bef8f4);
    Ptr r(p);
    FUN_08ad2810(&DAT_08bef8f4);
    return r;
}
