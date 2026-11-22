/* LBPMATCH module=EBOOT.BIN start=0x088d0070 end=0x088d0150 symbol=FUN_088d0070 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct State {
    int f0;
    int mode;
};

struct Slot {
    int f0;
    int f4;
    void *item;
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
    inline void *cur_item()
    {
        Slot *s = get_slot(idx);
        return (s != 0) ? s->item : 0;
    }
};

extern "C" Mgr *DAT_08beed14;
extern "C" void *DAT_08beed8c;
extern "C" void *FUN_088d7f9c(void);
extern "C" void FUN_088f73dc(void *);
extern "C" State *FUN_088dc4b4(Mgr *);
extern "C" void FUN_088e4cd4(void *, void *, int);
extern "C" void FUN_088e4de0(void *, void *, int);

extern "C" void FUN_088d0070(void)
{
    FUN_088f73dc(FUN_088d7f9c());
    if (FUN_088dc4b4(DAT_08beed14)->mode == 1) {
        FUN_088e4cd4(DAT_08beed8c, DAT_08beed14->cur_item(), 1);
    } else {
        FUN_088e4de0(DAT_08beed8c, DAT_08beed14->cur_item(), 1);
    }
}
