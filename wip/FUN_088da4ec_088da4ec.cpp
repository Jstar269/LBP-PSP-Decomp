/* LBPMATCH module=EBOOT.BIN start=0x088da4ec end=0x088da5d8 symbol=FUN_088da4ec lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Pair {
    unsigned int a;
    unsigned int b;
};

extern "C" void *FUN_08adb810(unsigned int, int, int, int);
extern "C" void FUN_088d71b4(void *, Pair *, int, int);

struct Elem {
    unsigned char pad[0x3ac];
};

struct Pool {
    Elem *base;
    Elem **stack;
    int cap;
    int top;
    int heap_count;

    inline Elem *alloc()
    {
        if (top < cap) {
            return stack[top++];
        }
        heap_count++;
        return (Elem *)FUN_08adb810(sizeof(Elem), 16, 0, 0);
    }
};

struct Holder {
    Pool *pool;
    Elem **arr;
    unsigned int cap;
    unsigned int count;
    unsigned int active;

    inline Elem *create(Pair pair, int a, int b)
    {
        Elem *p = pool->alloc();
        Elem *res = 0;
        if (active < cap) {
            active++;
            if (p != 0) {
                FUN_088d71b4(p, &pair, a, b);
                res = p;
            }
        }
        return res;
    }
};

struct Mgr {
    unsigned char pad[0x188];
    Holder holder;
};

extern "C" Elem *FUN_088da4ec(Mgr *param_1, const Pair *param_2, int param_3, int param_4)
{
    return param_1->holder.create(*param_2, param_3, param_4);
}

// Near-miss note: Branch/regalloc diff on 59/59 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
