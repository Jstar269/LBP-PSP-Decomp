/* LBPMATCH module=EBOOT.BIN start=0x088cf730 end=0x088cf7dc symbol=FUN_088cf730 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Handle {
    unsigned short idx;
};

struct Slot {
    int pad[10];
};

struct Mgr {
    unsigned char pad1[0x150];
    Slot *slots;
    unsigned char pad2[0x254 - 0x154];
    unsigned short cur;

    inline Slot *get_slot(int i)
    {
        return (i == 0xffff) ? 0 : &slots[i & 0xffff];
    }
    inline Slot *cur_slot()
    {
        return get_slot(cur);
    }
};

extern "C" Mgr *DAT_08beed14;
extern "C" void *DAT_08beee0c;
extern "C" void *DAT_08beed8c;
extern "C" int FUN_088f0d64(void *, void *, int);
extern "C" void FUN_088de4f0(Mgr *);
extern "C" void FUN_088dc31c(Mgr *);
extern "C" void FUN_088ea50c(void *, Slot *, int);
extern "C" void FUN_088e1a18(void *);

extern "C" void FUN_088cf730(void)
{
    if (FUN_088f0d64(DAT_08beee0c, (void *)FUN_088cf730, 1) == 0) {
        FUN_088de4f0(DAT_08beed14);
        FUN_088dc31c(DAT_08beed14);
        FUN_088ea50c(DAT_08beee0c, DAT_08beed14->cur_slot(), 10);
        FUN_088e1a18(DAT_08beed8c);
    }
}
