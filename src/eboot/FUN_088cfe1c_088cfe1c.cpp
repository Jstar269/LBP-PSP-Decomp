/* LBPMATCH module=EBOOT.BIN start=0x088cfe1c end=0x088cff04 symbol=FUN_088cfe1c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Item {
    int pad[10];
    unsigned int kind;
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
extern "C" void FUN_088e68a0(void *, int, int);
extern "C" void *FUN_088d7f9c(void);
extern "C" void FUN_088f73dc(void *);
extern "C" void FUN_088cfcec(void);
extern "C" void FUN_088cfd90(void);

extern "C" void FUN_088cfe1c(void)
{
    FUN_088e68a0(DAT_08beee0c, 0, 0x2a);
    Item *it = DAT_08beed14->cur_item();
    if (it != 0) {
        if (it->kind < 2) {
            FUN_088f73dc(FUN_088d7f9c());
            FUN_088e68a0(DAT_08beee0c, 1, 0x2b);
            FUN_088cfcec();
        } else if (it->kind == 2) {
            FUN_088f73dc(FUN_088d7f9c());
            FUN_088e68a0(DAT_08beee0c, 1, 0x2b);
            FUN_088cfd90();
        }
    }
}
