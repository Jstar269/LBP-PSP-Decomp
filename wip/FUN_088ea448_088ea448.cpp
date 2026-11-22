/* LBPMATCH module=EBOOT.BIN start=0x088ea448 end=0x088ea50c symbol=FUN_088ea448 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Ui {
    unsigned char pad1[0x40];
    void *f40;
    int f44;
    unsigned char pad2[0xa0 - 0x48];
    int fa0;
    unsigned char pad3[0xc0 - 0xa4];
    int fc0;
    unsigned char pad4[0xd0 - 0xc4];
    unsigned int pad_bits : 21;
    bool b21 : 1;
    unsigned int rest : 10;
};

extern "C" void FUN_0887f414(void *, int);
extern "C" void FUN_088ea1f0(Ui *, unsigned char, int);
extern "C" void FUN_088e68a0(Ui *, unsigned char, int);

extern "C" void FUN_088ea448(Ui *param_1, unsigned char param_2)
{
    if ((unsigned int)(param_1->f44 - 0x15) < 2U) {
        param_1->b21 = false;
        param_1->f44 = 0x19;
        param_1->fc0 = 0;
        FUN_0887f414(param_1->f40, 1);
        FUN_0887f414(param_1->f40, 2);
        if (param_1->fa0 != 0) {
            param_1->fa0 = 0;
        }
        if (param_2 == 0) {
            FUN_088ea1f0(param_1, 0, 0x2d);
            FUN_088ea1f0(param_1, 0, 0x2e);
            FUN_088e68a0(param_1, 1, 0x32);
        }
    }
}

// Near-miss note: 2-word scheduling tie; exact length.
// near miss from lane oc-lbp-20261121 (unreviewed)
