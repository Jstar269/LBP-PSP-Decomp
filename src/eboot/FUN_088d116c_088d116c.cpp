/* LBPMATCH module=EBOOT.BIN start=0x088d116c end=0x088d1210 symbol=FUN_088d116c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Item {
    unsigned char pad[0x6c];
    int id;
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
    inline Item *cur_item()
    {
        Slot *s = get_slot(idx);
        return (s != 0) ? s->item : 0;
    }
};

extern "C" Mgr *DAT_08beed14;
extern "C" void *DAT_08beee0c;
extern "C" unsigned char DAT_08bec4a8;
extern "C" unsigned char DAT_08bec4a9;
extern "C" int FUN_088e7e24(void *, void *);
extern "C" int FUN_088ed578(void *, int, int);
extern "C" void FUN_088d1034(void);

extern "C" void FUN_088d116c(void)
{
    if (FUN_088e7e24(DAT_08beee0c, (void *)FUN_088d116c) == 0) {
        Item *it = DAT_08beed14->cur_item();
        if (FUN_088ed578(DAT_08beee0c, 0xc00 - it->id, 0x242) != 0) {
            DAT_08bec4a9 = 1;
            DAT_08bec4a8 = 1;
            FUN_088d1034();
        }
    }
}
