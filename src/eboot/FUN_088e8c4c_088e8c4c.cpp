/* LBPMATCH module=EBOOT.BIN start=0x088e8c4c end=0x088e8d20 symbol=FUN_088e8c4c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void *FUN_08adb810(unsigned int, int, int, int);
extern "C" void FUN_08944a0c(void *);

struct Lock {
    int x;
};

struct Item {
    int pad[5];

    inline Item()
    {
        FUN_08944a0c(this);
    }
    inline void *operator new(unsigned int sz)
    {
        return FUN_08adb810(sz, 16, 0, 0);
    }
    inline void *operator new(unsigned int, void *p) throw()
    {
        return p;
    }
};

struct Pool {
    Item *base;
    Item **stack;
    int cap;
    int top;
    int heap_count;
};

struct Ref {
    Item *p;
    inline Ref(Item *x) : p(x) {}
    Ref(const Ref &);
};

extern "C" Lock DAT_08bef8f4;
extern "C" Pool DAT_08beee10;

extern "C" Ref FUN_088e8c4c(void)
{
    FUN_08ad3088(&DAT_08bef8f4);
    FUN_08ad3088(&DAT_08bef8f4);
    Item *obj;
    if (DAT_08beee10.top < DAT_08beee10.cap) {
        obj = new (DAT_08beee10.stack[DAT_08beee10.top++]) Item();
    } else {
        DAT_08beee10.heap_count++;
        obj = new Item();
    }
    FUN_08ad2810(&DAT_08bef8f4);
    Ref r(obj);
    FUN_08ad2810(&DAT_08bef8f4);
    return r;
}
