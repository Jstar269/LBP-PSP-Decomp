/* LBPMATCH module=EBOOT.BIN start=0x088ea7b8 end=0x088ea880 symbol=FUN_088ea7b8 lang=c++ */
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

struct Ui {
    unsigned char pad[0xd0];
    unsigned int pad_bits : 13;
    bool b13 : 1;
    bool b14 : 1;
    unsigned int rest : 17;
};

extern "C" Mgr *DAT_08beed14;
extern "C" void *DAT_08be85fc;
extern "C" void *FUN_08970dbc(void *, int);
extern "C" void *FUN_088d3344(void *, int);
extern "C" void *FUN_089ecaa8(void);
extern "C" void FUN_089ead84(void *, void *, void *, int, void *, void *, int);
extern "C" void FUN_088e62a8(void);
extern "C" void FUN_088ea060(void);

extern "C" void FUN_088ea7b8(Ui *param_1)
{
    void *it = DAT_08beed14->cur_item();
    void *title = FUN_08970dbc(DAT_08be85fc, 0x26f);
    void *text = FUN_088d3344(it, 1);
    FUN_089ead84(FUN_089ecaa8(), title, text, 0xd0, (void *)FUN_088e62a8, (void *)FUN_088ea060, 0);
    param_1->b13 = true;
    param_1->b14 = false;
}
