/* LBPMATCH module=EBOOT.BIN start=0x0889b0e8 end=0x0889b1ac symbol=FUN_0889b0e8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

inline void *operator new(unsigned, void *p) throw() { return p; }

extern "C" void FUN_0889aba0(void *, void *, void *);
extern "C" void FUN_0889a97c(void *, void *, void *);

struct Item {
    void (**vt)(Item *);
};

struct Vec {
    Item **begin;
    Item **end;
    Item **cap;
    inline void push_back(Item *const &x)
    {
        if (end != cap) {
            new (end) Item *(x);
            end++;
        } else {
            FUN_0889aba0(this, end, (void *)&x);
        }
    }
};

struct Obj {
    int pad[3];
    Vec v1;
    Vec v2;
};

extern "C" void FUN_0889b0e8(Obj *self)
{
    Vec *v2 = &self->v2;
    Vec *v1 = &self->v1;
    Item **p;
    for (p = v2->begin; p != v2->end; p++) {
        v1->push_back(*p);
    }
    FUN_0889a97c(v2, v2->begin, p);
    for (p = v1->begin; p != v1->end; p++) {
        (*p)->vt[2](*p);
    }
}
// near miss from lane oc-lbp-20261121 (unreviewed)
