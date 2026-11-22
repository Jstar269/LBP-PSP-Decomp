/* LBPMATCH module=EBOOT.BIN start=0x088cf648 end=0x088cf6e4 symbol=FUN_088cf648 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Slot {
    int pad[10];
};

struct Mgr {
    unsigned char pad1[0x150];
    Slot *slots;
    unsigned char pad2[0x254 - 0x154];
    unsigned short cur;

    inline Slot *get_slot(int idx)
    {
        return (idx == 0xffff) ? 0 : &slots[idx & 0xffff];
    }
    inline Slot *cur_slot()
    {
        return get_slot(cur);
    }
};

extern "C" Mgr *DAT_08beed14;
extern "C" void *DAT_08beee0c;
extern "C" void *DAT_08beed8c;
extern "C" int FUN_088f514c(void *, void *);
extern "C" void *FUN_088d7f9c(void);
extern "C" void FUN_088f7df8(void *);
extern "C" void FUN_088ea50c(void *, Slot *, int);
extern "C" void FUN_088e1a18(void *);

extern "C" void FUN_088cf648(void)
{
    if (FUN_088f514c(DAT_08beee0c, (void *)FUN_088cf648) == 0) {
        FUN_088f7df8(FUN_088d7f9c());
        FUN_088ea50c(DAT_08beee0c, DAT_08beed14->cur_slot(), 10);
        FUN_088e1a18(DAT_08beed8c);
    }
}
