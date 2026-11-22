/* LBPMATCH module=EBOOT.BIN start=0x088da790 end=0x088da83c symbol=FUN_088da790 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Desc {
    unsigned char pad1[0x10];
    unsigned short cat;
    unsigned char pad2[0x18 - 0x12];
};

struct Slot {
    unsigned short desc_idx;
    unsigned char pad[38];
};

struct Entry {
    int f0;
    int f4;
    unsigned int val;
    int pad[10];
};

struct Mgr {
    unsigned char pad1[0x148];
    Desc *descs;
    int pad14c;
    Slot *slots;
    int pad154[3];
    Entry *entries;

    inline Slot *get_slot(int idx)
    {
        return &slots[idx & 0xffff];
    }
    inline Entry *get_entry(int idx)
    {
        return &entries[idx & 0xffff];
    }
};

extern "C" void *FUN_08a50fbc(void);
extern "C" unsigned int FUN_08a52b38(void *, unsigned int);

extern "C" unsigned int FUN_088da790(Mgr *param_1, unsigned short param_2)
{
    if (param_2 != 0xffff) {
        Slot *s = param_1->get_slot(param_2);
        unsigned short cat = param_1->descs[s->desc_idx].cat;
        return FUN_08a52b38(FUN_08a50fbc(), param_1->get_entry(cat)->val);
    }
    return 0;
}
