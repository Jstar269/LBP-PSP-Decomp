/* LBPMATCH module=EBOOT.BIN start=0x0889b1ac end=0x0889b270 symbol=FUN_0889b1ac lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

inline void *operator new(unsigned, void *p) throw() { return p; }

extern "C" void FUN_0889a9e0(void *, void *);
extern "C" void FUN_0889aa54(void *, int);
extern "C" void FUN_0889aeb4(void *, void *, void *);

struct Item {
    void (**vt)(Item *);
    int state;
};

struct Vec {
    Item **begin;
    Item **end;
    Item **cap;
    inline unsigned int size() const { return (unsigned int)(end - begin); }
    inline Item *top()
    {
        if (begin == end || size() == 0) {
            return 0;
        }
        return *begin;
    }
    inline void push_back(Item *x)
    {
        Item **e = end;
        if (e != cap) {
            if (e != 0) {
                *e = x;
            }
            end = e + 1;
        } else {
            FUN_0889aeb4(this, e, &x);
        }
    }
};

extern "C" void FUN_0889b1ac(Vec *self)
{
    Item *p = self->top();
    if (p != 0) {
        FUN_0889a9e0(self, p);
        if (p->state != 2) {
            FUN_0889aa54(p, 2);
            p->vt[5](p);
        }
        self->push_back(p);
    }
}
// near miss from lane oc-lbp-20261121 (unreviewed)
