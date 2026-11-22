/* LBPMATCH module=EBOOT.BIN start=0x088d7db0 end=0x088d7e8c symbol=FUN_088d7db0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void FUN_08adb8a4(void *);

struct Elem {
    unsigned char pad[0x13d8];
};

struct Pool {
    Elem *base;
    Elem **stack;
    unsigned int cap;
    unsigned int top;
    int heap_count;

    inline bool owns(Elem *p)
    {
        return p >= base && p < base + cap;
    }
    inline void put(Elem *p)
    {
        unsigned int t = top - 1;
        stack[t] = p;
        top = t;
    }
    inline void release(Elem *p)
    {
        if (owns(p)) {
            put(p);
        } else if (p != 0) {
            heap_count--;
            FUN_08adb8a4(p);
        }
    }
};

struct Holder {
    Pool *pool;
    Elem **arr;
    int f8;
    unsigned int count;
    int active;

    inline void remove(Elem *p)
    {
        for (unsigned int i = 0; i < count; i++) {
            if (arr[i] == p) {
                arr[i] = arr[--count];
                break;
            }
        }
        if (--active != -1) {
            pool->release(p);
        }
    }
};

struct Mgr {
    unsigned char pad[0x19c];
    Holder holder;
};

extern "C" void FUN_088d7db0(Mgr *param_1, Elem *param_2)
{
    param_1->holder.remove(param_2);
}
