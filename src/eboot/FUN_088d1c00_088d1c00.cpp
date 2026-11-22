/* LBPMATCH module=EBOOT.BIN start=0x088d1c00 end=0x088d1ca4 symbol=FUN_088d1c00 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Sub {
    unsigned short pad[3];
    unsigned short f6;
};

struct Ui {
    unsigned char pad[0x18];
    Sub *sub;
};

struct Slot {
    int pad[10];
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

extern "C" Mgr *DAT_08beed14;
extern "C" void *DAT_08beee0c;
extern "C" void *DAT_08bedb54;
extern "C" void *DAT_08befcac;
extern "C" bool FUN_08842330(void *);
extern "C" void FUN_0895cce4(void *, int);
extern "C" void FUN_088de3e8(Mgr *, unsigned char);
extern "C" Ui *FUN_088d7f9c(void);
extern "C" void FUN_088f7950(Ui *, int, unsigned int);
extern "C" void FUN_088ea50c(void *, Slot *, int);

extern "C" void FUN_088d1c00(void)
{
    if (!FUN_08842330(DAT_08bedb54)) {
        FUN_0895cce4(DAT_08befcac, 5);
    }
    FUN_088de3e8(DAT_08beed14, 0);
    Ui *u = FUN_088d7f9c();
    FUN_088f7950(u, 0, u->sub->f6);
    FUN_088ea50c(DAT_08beee0c, DAT_08beed14->cur_slot(), 3);
}
