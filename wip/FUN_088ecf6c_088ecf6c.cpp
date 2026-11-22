/* LBPMATCH module=EBOOT.BIN start=0x088ecf6c end=0x088ed040 symbol=FUN_088ecf6c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct State {
    int f0;
    int mode;
};

struct Sub {
    unsigned short pad[3];
    unsigned short f6;
};

struct Info {
    unsigned char pad[0x18];
    Sub *sub;
};

struct Slot {
    int f0;
    int f4;
    int f8;
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
    unsigned char pad1[0x8c];
    unsigned char f8c;
    unsigned char pad2[0xd0 - 0x8d];
    unsigned int pad_bits : 15;
    bool b15 : 1;
    unsigned int rest : 16;
};

extern "C" Mgr *DAT_08beed14;
extern "C" State *FUN_088dc4b4(Mgr *);
extern "C" Info *FUN_088d7f9c(void);
extern "C" void FUN_088ece00(Ui *, unsigned char);

extern "C" bool FUN_088ecf6c(Ui *param_1)
{
    param_1->b15 = false;
    if (FUN_088dc4b4(DAT_08beed14)->mode == 3 && FUN_088d7f9c()->sub->f6 == 0xffff) {
        Slot *s = DAT_08beed14->cur_slot();
        if (s->f4 == 0 && s->f8 == 0 && param_1->f8c == 0) {
            FUN_088ece00(param_1, 1);
            param_1->b15 = true;
        }
    }
    return param_1->b15;
}

// Near-miss note: Register/branch layout diff on 52/53 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
