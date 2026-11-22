/* LBPMATCH module=EBOOT.BIN start=0x088d8960 end=0x088d8a44 symbol=FUN_088d8960 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Elem {
    unsigned char pad[0x3ac];
};

struct Pool {
    Elem *base;
    Elem **stack;
    int cap;
    int top;
    int heap_count;
    unsigned char flag;
};

extern "C" void FUN_08adb8a4(void *);
extern "C" void *FUN_08adb810(unsigned int, int, int, int);

extern "C" void FUN_088d8960(Pool *param_1, int param_2, unsigned char param_3)
{
    if (param_1->base != 0) {
        FUN_08adb8a4(param_1->base);
    }
    param_1->base = 0;
    param_1->stack = 0;
    param_1->cap = param_2;
    param_1->top = 0;
    if (param_2 > 0) {
        unsigned int bytes = ((unsigned int)(param_2 * sizeof(Elem)) + 15U) & ~15U;
        void *mem = FUN_08adb810(bytes + (unsigned int)(param_2 * 4), 16, 0, 0);
        int cap = param_1->cap;
        unsigned int off = ((unsigned int)(cap * sizeof(Elem)) + 15U) & ~15U;
        Elem **st = (Elem **)((char *)mem + off);
        param_1->base = (Elem *)mem;
        param_1->stack = st;
        for (int i = 0; i < cap; i++) {
            st[i] = param_1->base + i;
        }
    }
    param_1->flag = param_3;
}

// Near-miss note: Retail uses mult * 940 (0x3ac); GCC expands to shifts/subs.
// near miss from lane oc-lbp-20261121 (unreviewed)
