/* LBPMATCH module=EBOOT.BIN start=0x088e6e7c end=0x088e6f24 symbol=FUN_088e6e7c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Mgr {
    unsigned char pad[0x1c4];
    void *f1c4;
};

struct Net {
    unsigned char pad[0x55];
    unsigned char f55;
};

struct Ui {
    unsigned char pad1[0x44];
    int f44;
    unsigned char pad2[0xa0 - 0x48];
    int fa0;
    unsigned char pad3[0xd0 - 0xa4];
    unsigned int pad_bits : 22;
    bool b22 : 1;
    unsigned int rest_bits : 9;
};

extern "C" Mgr *DAT_08beed14;
extern "C" Net *DAT_08beed8c;
extern "C" void FUN_088e5f80(Net *, void *, int);
extern "C" void FUN_088e5170(Net *);

extern "C" int FUN_088e6e7c(Ui *param_1, int param_2)
{
    if (param_1->f44 != 0x19) {
        if (DAT_08beed8c->f55 != 0) {
            return 0;
        }
        if ((unsigned int)(param_1->f44 - 0x12) >= 3U) {
            if (param_1->b22) {
                FUN_088e5f80(DAT_08beed8c, DAT_08beed14->f1c4, 1);
            } else {
                FUN_088e5170(DAT_08beed8c);
            }
        }
    }
    if (param_2 != param_1->fa0) {
        param_1->fa0 = param_2;
    }
    return 1;
}

// Near-miss note: Branch/regalloc diff on 41/42 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
