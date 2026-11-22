/* LBPMATCH module=EBOOT.BIN start=0x088f36b8 end=0x088f3748 symbol=FUN_088f36b8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Item {
    unsigned char pad[0x38];
    int f38;
};

struct Ui {
    unsigned char pad1[0x40];
    void *f40;
    int f44;
    int f48;
    unsigned char pad2[0xd0 - 0x4c];
    unsigned int pad_bits : 21;
    bool b21 : 1;
    unsigned int rest : 10;
};

struct Arg {
    int id;
    int pad[7];
};

extern "C" void FUN_088ea448(Ui *, unsigned char);
extern "C" void FUN_08881018(void *, int, Arg *, unsigned char);

extern "C" void FUN_088f36b8(Ui *param_1, Item *param_2)
{
    param_1->b21 = false;
    if (param_2 != 0 && param_2->f38 != 0xffff && param_2->f38 != 0) {
        Arg a;
        a.id = param_2->f38;
        param_1->f48 = 1;
        FUN_08881018(param_1->f40, 2, &a, 0);
        param_1->f44 = 0x16;
    } else {
        FUN_088ea448(param_1, 0);
    }
    param_1->b21 = true;
}

// Near-miss note: Else-block cold placement after epilogue (35/36 words); flipped-if and duplicated-store tried.
// near miss from lane oc-lbp-20261121 (unreviewed)
