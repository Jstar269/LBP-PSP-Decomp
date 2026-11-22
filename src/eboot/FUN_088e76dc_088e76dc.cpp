/* LBPMATCH module=EBOOT.BIN start=0x088e76dc end=0x088e77b8 symbol=FUN_088e76dc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Item {
    unsigned char pad[0x7c];
    unsigned short f7c;
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
    unsigned char pad2[0x1ce - 0x154];
    unsigned char f1ce;
    unsigned char pad3[0x254 - 0x1cf];
    unsigned short idx;

    inline Slot *get_slot(int i)
    {
        return (i == 0xffff) ? 0 : &slots[i & 0xffff];
    }
    inline Slot *cur_slot()
    {
        return get_slot(idx);
    }
};

struct Ui {
    unsigned char pad[0xd0];
    unsigned int pad_bits : 25;
    unsigned int b25 : 1;
    unsigned int rest : 6;
    void (*cb)(void);
    int fd8;
    Item *fdc;
};

extern "C" Mgr *DAT_08beed14;
extern "C" void *DAT_08beee0c;
extern "C" void FUN_088e68a0(void *, unsigned char, int);

extern "C" void FUN_088e76dc(Ui *param_1, unsigned char param_2)
{
    if (param_2 == 0) {
        Slot *s = DAT_08beed14->cur_slot();
        if (s != 0 && s->item != 0 && s->item != param_1->fdc) {
            FUN_088e68a0(DAT_08beee0c, 1, 0x3a);
            return;
        }
    }
    if (param_1->fdc != 0 && DAT_08beed14->idx != param_1->fdc->f7c) {
        param_1->cb();
    }
    param_1->b25 = 0;
    param_1->cb = 0;
    DAT_08beed14->f1ce = 1;
}
