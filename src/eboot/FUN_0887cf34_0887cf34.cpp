/* LBPMATCH module=EBOOT.BIN start=0x0887cf34 end=0x0887d014 symbol=FUN_0887cf34 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); matching idiom: volatile on Sub::state; exact match verified; names and types await human review. */

extern "C" void FUN_08878dc8(void *);
extern "C" int FUN_0887c14c(void *, void *, int);

struct Sub {
    int f0;
    volatile int state;
    inline void set_state(int s) { state = s; }
};

struct ItemVT {
    void *f[9];
    int (*fn9)(void *);
    void (*fn10)(void *);
};

struct Base {
    ItemVT *vt;
    int f4;
    int f8;
};

struct Item : Base, Sub {
};

struct Obj {
    unsigned char pad[448];
    unsigned char flag;
};

extern "C" void FUN_0887cf34(Obj *self, Item **param_2)
{
    if (self->flag != 0) {
        FUN_08878dc8(*param_2);
        (*param_2)->set_state(5);
        return;
    }
    if ((*param_2)->vt->fn9(*param_2) != 0) {
        int r = FUN_0887c14c(self, *param_2, 0);
        Item *p = *param_2;
        if ((unsigned)(r - 4) < 2) {
            FUN_08878dc8(p);
            p->set_state(r);
        } else {
            p->vt->fn10(p);
            p->set_state(6);
        }
        return;
    }
    FUN_08878dc8(*param_2);
    (*param_2)->set_state(6);
}
