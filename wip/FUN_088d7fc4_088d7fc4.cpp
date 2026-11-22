/* LBPMATCH module=EBOOT.BIN start=0x088d7fc4 end=0x088d80a4 symbol=FUN_088d7fc4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct App {
    unsigned char pad[0x195];
    unsigned char f195;
};

struct Ui {
    unsigned char pad[0x4c];
    int f4c;
};

struct Item {
    int pad[10];
    unsigned int kind;
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
    unsigned char pad2[0x1c4 - 0x154];
    void *f1c4;
    void *f1c8;
    int pad3;
    int f1d0;
    unsigned char pad4[0x254 - 0x1d4];
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

extern "C" Ui *DAT_08beee0c;
extern "C" App *FUN_089ecaa8(void);
extern "C" void FUN_088cd31c(void *);
extern "C" void FUN_088d1e30(void *, unsigned char);
extern "C" void FUN_088d77fc(Mgr *);
extern "C" bool FUN_088d3560(Item *);
extern "C" void FUN_088d6a70(Item *);

extern "C" void FUN_088d7fc4(Mgr *param_1)
{
    if (FUN_089ecaa8()->f195 == 0 && DAT_08beee0c->f4c != 1) {
        param_1->f1d0 = 0;
        FUN_088cd31c(param_1->f1c4);
        FUN_088d1e30(param_1->f1c8, 0);
        FUN_088d77fc(param_1);
        Slot *s = param_1->cur_slot();
        if (s != 0 && s->item != 0) {
            Item *it = s->item;
            unsigned int k = it->kind;
            if (k == 3 || k == 5 || k == 4) {
                return;
            }
            if (FUN_088d3560(it)) {
                FUN_088d6a70(it);
            }
        }
    }
}

// Near-miss note: List/pool traversal regalloc diff on 54/56 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
