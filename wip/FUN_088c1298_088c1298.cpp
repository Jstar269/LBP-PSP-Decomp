/* LBPMATCH module=EBOOT.BIN start=0x088c1298 end=0x088c1358 symbol=FUN_088c1298 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" int DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);

struct Sub {
    void (**vt)(Sub *);
};

struct Task {
    void (**vt)(Task *);
    int rc;
};

struct TaskPtr {
    Task *p;
    inline void clear()
    {
        Task *r = p;
        p = 0;
        if (r != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            int c = --r->rc;
            FUN_08ad2810(&DAT_08bef8f4);
            if (c == 0) {
                r->vt[2](r);
            }
        }
    }
};

struct SlotHolder {
    int pad[3];
    TaskPtr arr[8];
};

struct Obj {
    int pad1[5];
    Sub *sub;
    unsigned char f18;
    unsigned char f19;
    unsigned char pad2[0x60 - 0x1a];
    SlotHolder slots;
    int count;
    int idx;
};

extern "C" void FUN_088c1298(Obj *self)
{
    self->sub->vt[14](self->sub);
    self->slots.arr[self->idx].clear();
    if (++self->idx >= self->count) {
        self->f19 = 1;
    }
}

// Remaining diffs (48/48 words; 3 differ at +0x3c): GCC splits offset 0x6c as +104 + 4 instead of retail's +96 + 12 (same address, different base/disp split).
// near miss from lane oc-lbp-20261121 (unreviewed)
