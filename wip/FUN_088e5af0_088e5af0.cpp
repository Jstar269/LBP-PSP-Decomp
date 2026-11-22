/* LBPMATCH module=EBOOT.BIN start=0x088e5af0 end=0x088e5b9c symbol=FUN_088e5af0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct State {
    int f0;
    int mode;
};

struct Mgr {
    unsigned char pad[0x1ce];
    unsigned char f1ce;
};

struct Sub {
    int f0;
    int f4;
    unsigned int f8;
    int pad[4];
};

struct Net {
    Sub s0;
    Sub s1;
    unsigned char pad[0x57 - 0x38];
    unsigned char f57;
};

extern "C" Mgr *DAT_08beed14;
extern "C" State *FUN_088dc4b4(Mgr *);
extern "C" void FUN_088e4fc8(Net *);
extern "C" void FUN_088e5248(Net *, int, int);

extern "C" void FUN_088e5af0(Net *param_1)
{
    param_1->f57 = 0;
    if (FUN_088dc4b4(DAT_08beed14)->mode == 1) {
        if (param_1->s0.f8 < 2) {
            return;
        }
        FUN_088e5248(param_1, param_1->s0.f0, param_1->s0.f8 - 1);
    } else {
        if (param_1->s1.f8 >= 2) {
            FUN_088e5248(param_1, param_1->s1.f0, param_1->s1.f8 - 1);
        } else {
            FUN_088e4fc8(param_1);
            return;
        }
    }
    DAT_08beed14->f1ce = 1;
}

// Near-miss note: Branch/regalloc diff on 39/43 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
