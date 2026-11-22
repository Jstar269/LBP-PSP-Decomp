/* LBPMATCH module=EBOOT.BIN start=0x088d16b4 end=0x088d17a8 symbol=FUN_088d16b4 lang=c++ */
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

extern "C" void FUN_088d1634(void *, int, void **);
extern "C" void FUN_08970b6c(void *);

struct Msg {
    void *p;
    int f4;
    int f8;

    inline Msg(int code, void *arg)
    {
        FUN_088d1634(this, code, &arg);
    }
    inline ~Msg()
    {
        if (p != 0) {
            FUN_08970b6c(p);
        }
    }
};

extern "C" void FUN_0896fbd4(void *, const Msg &, int, int);

struct Bus {
    inline void send(const Msg &m, int a, int b)
    {
        FUN_0896fbd4(this, m, a, b);
    }
};

extern "C" Mgr *DAT_08beed14;
extern "C" void *DAT_08beee0c;
extern "C" Bus *DAT_08bf03b8;
extern "C" int FUN_088e77d0(void *, void *);
extern "C" void FUN_088e68a0(void *, unsigned char, int);
extern "C" void FUN_08970494(Bus *, int, int);

extern "C" void FUN_088d16b4(void)
{
    if (FUN_088e77d0(DAT_08beee0c, (void *)FUN_088d16b4) != 0) {
        FUN_088e68a0(DAT_08beee0c, 1, 0x17);
    } else {
        FUN_088e68a0(DAT_08beee0c, 0, 0x17);
        DAT_08bf03b8->send(Msg(22000, DAT_08beed14->cur_item()), 1, 6);
        FUN_08970494(DAT_08bf03b8, 1, 6);
    }
}
