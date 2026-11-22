/* LBPMATCH module=EBOOT.BIN start=0x088e7d4c end=0x088e7e24 symbol=FUN_088e7d4c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

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
    unsigned char pad[0xd0];
    unsigned int pad_bits : 25;
    bool b25 : 1;
    unsigned int rest : 6;
};

extern "C" Mgr *DAT_08beed14;
extern "C" State *FUN_088dc4b4(Mgr *);
extern "C" Info *FUN_088d7f9c(void);

extern "C" int FUN_088e7d4c(Ui *param_1)
{
    Slot *s = DAT_08beed14->cur_slot();
    if (s != 0) {
        if (s->f4 != 0) {
            return 1;
        }
        if (s->f8 != 0) {
            return 1;
        }
        if (FUN_088dc4b4(DAT_08beed14)->mode == 4) {
            return 1;
        }
        if (FUN_088dc4b4(DAT_08beed14)->mode == 3 && FUN_088d7f9c()->sub->f6 == 0xffff &&
            !param_1->b25) {
            return 1;
        }
    }
    return 0;
}
