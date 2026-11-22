/* LBPMATCH module=EBOOT.BIN start=0x088e86a8 end=0x088e8768 symbol=FUN_088e86a8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Item {
    unsigned int flags;
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
    unsigned char pad2[0x254 - 0x154];
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
    unsigned char pad1[0x44];
    int f44;
    int f48;
    unsigned char pad2[0xa0 - 0x4c];
    int fa0;
    int fa4;
    int fa8;
    int fac;
};

extern "C" Mgr *DAT_08beed14;

extern "C" int FUN_088e86a8(Ui *param_1, int param_2)
{
    if (param_1->f48 != 0) {
        if (param_2 != param_1->fa0) {
            param_1->fa0 = param_2;
        }
        return 1;
    }
    if (param_1->f44 == 0x19) {
        if (param_2 != param_1->fa0) {
            param_1->fa0 = param_2;
        }
        return 1;
    }
    Slot *s = DAT_08beed14->cur_slot();
    if (s != 0 && s->item != 0 && (s->item->flags & 0x10000) != 0) {
        if (param_2 != param_1->fac) {
            param_1->fac = param_2;
        }
        return 1;
    }
    if (param_1->fa4 != 0) {
        param_1->fa4 = 0;
    }
    if (param_1->fa0 != 0) {
        param_1->fa0 = 0;
    }
    return 0;
}

// Near-miss note: Register/branch layout diff on 47/48 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
