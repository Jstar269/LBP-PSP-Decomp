/* LBPMATCH module=EBOOT.BIN start=0x088d1034 end=0x088d1120 symbol=FUN_088d1034 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Sub {
    int f0;
    int count;
    int pad[3];
    unsigned char dirty;
};

struct Ui {
    unsigned char pad[0x4c];
    int f4c;
};

struct Item {
    int pad1[3];
    int f0c;
    int pad2[6];
    int kind;
};

struct Slot {
    int f0;
    int f4;
    Item *item;
    int pad[7];
};

struct Mgr {
    unsigned char pad1[0x150];
    Slot *slots;
    unsigned char pad2[0x1c8 - 0x154];
    Sub *sub;
    unsigned char pad3[0x254 - 0x1cc];
    unsigned short idx;

    inline Slot *get_slot(int i)
    {
        return (i == 0xffff) ? 0 : &slots[i & 0xffff];
    }
    inline Item *cur_item()
    {
        Slot *s = get_slot(idx);
        return (s != 0) ? s->item : 0;
    }
};

extern "C" Mgr *DAT_08beed14;
extern "C" Ui *DAT_08beee0c;
extern "C" void FUN_088d7fc4(Mgr *);
extern "C" void *FUN_088d7f9c(void);
extern "C" void FUN_088f73dc(void *);
extern "C" void FUN_088e68a0(Ui *, unsigned char, int);
extern "C" void FUN_088d0f00(void);

extern "C" void FUN_088d1034(void)
{
    Mgr *m = DAT_08beed14;
    Sub *sub = m->sub;
    sub->dirty = 1;
    int n = ++sub->count;
    FUN_088d7fc4(m);
    DAT_08beee0c->f4c = 1;
    Item *it = DAT_08beed14->cur_item();
    int k = it->kind;
    if (k == 3 || k == 5 || k == 4) {
        it->f0c = n;
    } else if (k == 1) {
        it->f0c = n;
    }
    FUN_088f73dc(FUN_088d7f9c());
    FUN_088e68a0(DAT_08beee0c, 1, 0x17);
    FUN_088d0f00();
}

// Near-miss note: GCC folds identical store arms for kind==3/5/4 vs kind==1.
// near miss from lane oc-lbp-20261121 (unreviewed)
