/* LBPMATCH module=EBOOT.BIN start=0x088f71f0 end=0x088f72d8 symbol=FUN_088f71f0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Sub {
    unsigned char pad[0xe];
    unsigned short f0e;
};

struct Page {
    unsigned char pad[0x18];
    Sub *sub;
};

struct Slot {
    int f0;
    void *f4;
    void *f8;
    int pad[7];
};

struct Mgr {
    unsigned char pad[0x150];
    Slot *slots;

    inline Slot *get_slot(int i)
    {
        return &slots[i & 0xffff];
    }
};

extern "C" void FUN_088d1634(void *, int, void **);
extern "C" void FUN_088e447c(void *, int, void **);
extern "C" void FUN_08970b6c(void *);

struct MsgA {
    void *p;
    inline MsgA(int code, void **arg) { FUN_088d1634(this, code, arg); }
    inline ~MsgA()
    {
        if (p != 0) {
            FUN_08970b6c(p);
        }
    }
};

struct MsgB {
    void *p;
    int f4;
    int f8;
    inline MsgB(int code, void **arg) { FUN_088e447c(this, code, arg); }
    inline ~MsgB()
    {
        if (p != 0) {
            FUN_08970b6c(p);
        }
    }
};

extern "C" void FUN_0896fc44(void *, const void *);

struct Bus {
    inline void send(const MsgA &m) { FUN_0896fc44(this, &m); }
    inline void send(const MsgB &m) { FUN_0896fc44(this, &m); }
};

extern "C" unsigned char DAT_08beee5e;
extern "C" unsigned char DAT_08beee5c;
extern "C" Mgr *DAT_08beed14;
extern "C" Bus *DAT_08bf03b8;

extern "C" void FUN_088f71f0(Page *param_1, unsigned char param_2)
{
    if (DAT_08beee5e == 0 && DAT_08beee5c == 0) {
        unsigned short idx = param_1->sub->f0e;
        if (idx != 0xffff) {
            Slot *s = DAT_08beed14->get_slot(idx);
            if (s->f4 != 0 || param_2 != 0) {
                DAT_08bf03b8->send(MsgB(32000, &s->f4));
            } else {
                DAT_08bf03b8->send(MsgA(22000, &s->f8));
            }
        }
    }
}
