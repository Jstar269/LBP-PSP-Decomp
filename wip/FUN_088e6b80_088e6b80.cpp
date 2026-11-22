/* LBPMATCH module=EBOOT.BIN start=0x088e6b80 end=0x088e6c10 symbol=FUN_088e6b80 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Obj {
    unsigned char pad1[0x34];
    void *f34;
    unsigned char pad2[0xd0 - 0x38];
    bool b0 : 1;
    bool b1 : 1;
    bool b2 : 1;
    bool b3 : 1;
    unsigned int rest : 28;
};

extern "C" void *DAT_08befcac;
extern "C" void FUN_08962884(void *);
extern "C" int FUN_08961c28(void *);
extern "C" void FUN_0895cba8(void *, unsigned char);

extern "C" void FUN_088e6b80(Obj *param_1)
{
    if (!param_1->b1) {
        FUN_08962884(param_1->f34);
        if (!param_1->b1 && !param_1->b2 && !param_1->b3 && FUN_08961c28(param_1->f34) != 0) {
            param_1->b2 = true;
            FUN_0895cba8(DAT_08befcac, 0);
        }
    }
}

// Near-miss note: Branch/regalloc diff on 34/36 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
