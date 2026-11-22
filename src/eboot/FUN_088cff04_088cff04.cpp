/* LBPMATCH module=EBOOT.BIN start=0x088cff04 end=0x088cffb4 symbol=FUN_088cff04 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Item {
    unsigned char pad[0x6c];
    unsigned int id;
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
extern "C" int FUN_088e7e24(void *, void *);
extern "C" int FUN_088ed578(void *, unsigned int, int);
extern "C" void FUN_088e759c(void *, void (*)(void), int);
extern "C" void FUN_088cf5f0(void);
extern "C" void FUN_088dbfa8(Mgr *);

extern "C" void FUN_088cff04(void)
{
    if (FUN_088e7e24(DAT_08beee0c, (void *)FUN_088cff04) == 0) {
        Item *it = DAT_08beed14->cur_item();
        if (FUN_088ed578(DAT_08beee0c, it->id, 0x36f) != 0) {
            FUN_088e759c(DAT_08beee0c, FUN_088cf5f0, 0x28d);
            FUN_088dbfa8(DAT_08beed14);
        }
    }
}
