/* LBPMATCH module=EBOOT.BIN start=0x088cfb84 end=0x088cfc38 symbol=FUN_088cfb84 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

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
extern "C" void *DAT_08beee0c;
extern "C" void FUN_088e68a0(void *, int, int);
extern "C" void FUN_088ea1f0(void *, int, int);
extern "C" int FUN_088e86a8(void *, void *);
extern "C" void FUN_088f4724(void *, void *);

extern "C" void FUN_088cfb84(void)
{
    FUN_088e68a0(DAT_08beee0c, 0, 0x33);
    FUN_088e68a0(DAT_08beee0c, 0, 0x34);
    FUN_088ea1f0(DAT_08beee0c, 1, 0x2d);
    if (FUN_088e86a8(DAT_08beee0c, (void *)FUN_088cfb84) == 0) {
        FUN_088f4724(DAT_08beee0c, DAT_08beed14->cur_item());
    }
}
