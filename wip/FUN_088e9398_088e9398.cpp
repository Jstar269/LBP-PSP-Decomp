/* LBPMATCH module=EBOOT.BIN start=0x088e9398 end=0x088e9438 symbol=FUN_088e9398 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Page {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    int f4;
    int f8;
    unsigned int flags;
};

extern "C" const char DAT_08bb4b78[];
extern "C" void FUN_08b887b0(const char *) __attribute__((noreturn));

struct Vec {
    Page **begin;
    Page **end;
    Page **cap;

    inline unsigned int size()
    {
        return (unsigned int)(end - begin);
    }
    inline Page *at(unsigned int i)
    {
        if (i >= size()) {
            FUN_08b887b0(DAT_08bb4b78);
        }
        return begin[i];
    }
};

struct Stack {
    int pad[2];
    Vec active;
};

extern "C" void FUN_088e9398(Stack *param_1)
{
    if (param_1->active.begin == param_1->active.end) {
        return;
    }
    if ((param_1->active.end[-1]->flags & 4) != 0) {
        unsigned int sz = param_1->active.size();
        if (sz > 1) {
            param_1->active.at(sz - 2)->v10();
        }
    }
    param_1->active.end[-1]->v10();
}

// Near-miss note: Branch/regalloc diff on 42/40 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
