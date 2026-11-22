/* LBPMATCH module=EBOOT.BIN start=0x088d0e0c end=0x088d0f00 symbol=FUN_088d0e0c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct State {
    int f0;
    int mode;
};

struct Net;
extern "C" void FUN_088e3a30(Net *, void *, int);
extern "C" void FUN_088e5f80(Net *, void *, int);

struct Net {
    unsigned char pad[0x56];
    unsigned char f56;

    inline void op1(void *p, int a)
    {
        FUN_088e3a30(this, p, a);
    }
    inline void op2(void *p, int a)
    {
        FUN_088e5f80(this, p, a);
    }
};

struct Slot {
    int f0;
    void *f4;
    int pad[8];
};

struct Mgr {
    unsigned char pad1[0x150];
    Slot *slots;
    unsigned char pad2[0x254 - 0x154];
    unsigned short idx;

    inline Slot *get_slot(int i)
    {
        return (i == 0xffff) ? 0 : &slots[i & 0xffff];
    }
    inline void *cur_f4()
    {
        Slot *s = get_slot(idx);
        return (s != 0) ? s->f4 : 0;
    }
};

extern "C" Mgr *DAT_08beed14;
extern "C" Net *DAT_08beed8c;
extern "C" void *FUN_088d7f9c(void);
extern "C" void FUN_088f73dc(void *);
extern "C" State *FUN_088dc4b4(Mgr *);

extern "C" void FUN_088d0e0c(void)
{
    FUN_088f73dc(FUN_088d7f9c());
    Mgr *m = DAT_08beed14;
    DAT_08beed8c->f56 = 1;
    if (FUN_088dc4b4(m)->mode == 1) {
        DAT_08beed8c->op1(DAT_08beed14->cur_f4(), 1);
    } else {
        DAT_08beed8c->op2(DAT_08beed14->cur_f4(), 1);
    }
}

// Near-miss note: 2-word local scheduling diff on store order; exact length.
// near miss from lane oc-lbp-20261121 (unreviewed)
