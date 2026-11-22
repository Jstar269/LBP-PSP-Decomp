/* LBPMATCH module=EBOOT.BIN start=0x088de4f0 end=0x088de5dc symbol=FUN_088de4f0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Net {
    int f0;
    unsigned char pad[0x54 - 4];
    unsigned char f54;
};

struct Sub {
    int pad[2];
    int f8;
};

struct Mgr {
    int pad1[2];
    int mode;
    unsigned char pad2[0x168 - 0xc];
    int count;
    unsigned char pad3[0x178 - 0x16c];
    unsigned short start;
    unsigned char pad4[0x194 - 0x17a];
    int f194;
    unsigned char pad5[0x1c8 - 0x198];
    Sub *sub;
};

extern "C" Net *DAT_08beed8c;
extern "C" int FUN_088e1d90(Net *);
extern "C" void FUN_088d1e30(Sub *, unsigned char);
extern "C" void FUN_088dd3fc(Mgr *, unsigned short, unsigned char);
extern "C" void FUN_088e34f4(Net *, int);

extern "C" void FUN_088de4f0(Mgr *param_1)
{
    if (param_1->mode == 2 && DAT_08beed8c->f0 != -1 && DAT_08beed8c->f54 == 0 &&
        FUN_088e1d90(DAT_08beed8c) == 0) {
        return;
    }
    Sub *sub = param_1->sub;
    unsigned short i;
    if (param_1->f194 != sub->f8) {
        sub->f8 = param_1->f194;
        FUN_088d1e30(sub, 1);
    }
    i = param_1->start;
    param_1->mode = 2;
    unsigned short end = (unsigned short)(i + param_1->count - 1);
    for (; i <= end; i++) {
        FUN_088dd3fc(param_1, i, 0);
    }
    FUN_088e34f4(DAT_08beed8c, 1);
    DAT_08beed8c->f54 = 0;
}
