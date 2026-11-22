/* LBPMATCH module=EBOOT.BIN start=0x088d80a4 end=0x088d8180 symbol=FUN_088d80a4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern "C" void *FUN_08adb810(unsigned int, int, int, int);
extern "C" void FUN_088ce44c(void *, unsigned int);

struct Elem {
    unsigned char pad[0x13d8];
    inline void *operator new(unsigned int, void *p) throw() { return p; }
    inline Elem(unsigned int arg) { FUN_088ce44c(this, arg); }
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

    inline Elem *create(unsigned int arg)
    {
        Elem *p = pool->alloc();
        if (active < cap) {
            active++;
            arr[count++] = p;
            return new (p) Elem(arg);
        }
        return 0;
    }
};

struct Mgr {
    unsigned char pad[0x19c];
    Holder holder;
};

extern "C" Elem *FUN_088d80a4(Mgr *param_1, unsigned int param_2)
{
    return param_1->holder.create(param_2);
}

// Near-miss note: Pool placement-new ctor regalloc/null-check diff.
// near miss from lane oc-lbp-20261121 (unreviewed)
