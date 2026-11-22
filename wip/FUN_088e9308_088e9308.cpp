/* LBPMATCH module=EBOOT.BIN start=0x088e9308 end=0x088e9398 symbol=FUN_088e9308 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Page {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
};

inline void *operator new(unsigned int, void *p) throw()
{
    return p;
}

struct Vec;
extern "C" void FUN_088e913c(Vec *, Page **, Page **);

struct Vec {
    Page **begin;
    Page **end;
    Page **cap;

    inline void push_back(Page *&p)
    {
        if (end != cap) {
            new (end) Page *(p);
            end++;
        } else {
            FUN_088e913c(this, end, &p);
        }
    }
};

struct Stack {
    int pad[2];
    Vec active;
    Vec removed;
};

extern "C" void FUN_088e9308(Stack *param_1)
{
    if (param_1->active.begin != param_1->active.end) {
        Page *p = *--param_1->active.end;
        p->v3();
        param_1->removed.push_back(p);
    }
}

// Near-miss note: Branch/regalloc diff on 35/36 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
