/* LBPMATCH module=EBOOT.BIN start=0x088d2940 end=0x088d29d0 symbol=FUN_088d2940 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Buf {
    unsigned char pad[0x16];
    short f16;
    short f18;
};

struct Item {
    int pad[10];
    unsigned int kind;
};

extern "C" unsigned char DAT_08bee7b4[];
extern "C" Buf *DAT_08beecb4;
extern "C" void FUN_088d5108(void *, Item *);
extern "C" void FUN_088d6da8(void *, Item *, unsigned int, Item *);

extern "C" void FUN_088d2940(Item *param_1)
{
    unsigned int k = param_1->kind;
    if (k == 3 || k == 5 || k == 4) {
        FUN_088d5108(DAT_08bee7b4, param_1);
        DAT_08beecb4 = (Buf *)DAT_08bee7b4;
    } else {
        FUN_088d6da8(DAT_08bee7b4, param_1, k, param_1);
        DAT_08beecb4 = (Buf *)DAT_08bee7b4;
    }
    DAT_08beecb4->f16 = 0;
    DAT_08beecb4->f18 = 0;
}

// Near-miss note: Register allocation / branch ordering diff on 36/36 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
