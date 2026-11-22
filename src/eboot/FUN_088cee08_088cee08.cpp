/* LBPMATCH module=EBOOT.BIN start=0x088cee08 end=0x088ceecc symbol=FUN_088cee08 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Slot {
    int f0;
    int f4;
    void *f8;
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
        return (s != 0) ? s->f8 : 0;
    }
};

extern "C" Mgr *DAT_08beed14;
extern "C" void *DAT_08beee0c;
extern "C" bool FUN_088e6388(void *);
extern "C" bool FUN_088d3560(void *);
extern "C" void FUN_088d6a70(void *);
extern "C" int FUN_088f6650(void *, void *);
extern "C" void FUN_088e8d20(void *, int);
extern "C" void FUN_088e657c(void *);

extern "C" void FUN_088cee08(void)
{
    if (!FUN_088e6388(DAT_08beee0c)) {
        return;
    }
    void *p = DAT_08beed14->cur_item();
    if (FUN_088d3560(p)) {
        FUN_088d6a70(p);
    }
    if (FUN_088f6650(DAT_08beee0c, (void *)FUN_088cee08) != 0) {
        FUN_088e657c(DAT_08beee0c);
    } else {
        FUN_088e8d20(DAT_08beee0c, 1);
    }
}
