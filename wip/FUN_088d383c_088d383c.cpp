/* LBPMATCH module=EBOOT.BIN start=0x088d383c end=0x088d3918 symbol=FUN_088d383c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Item {
    unsigned char pad1[0x28];
    int kind;
    unsigned char pad2[0xe0 - 0x2c];
    char names[16][17];

    inline char *get_names()
    {
        if (kind == 3 || kind == 5 || kind == 4) {
            return &names[0][0];
        }
        return &names[0][0];
    }
};

extern "C" unsigned char DAT_08beecb1;
extern "C" void *DAT_08be85fc;
extern "C" char DAT_08bb4318[];
extern "C" void FUN_08882764(void);
extern "C" unsigned short *FUN_08970dbc(void *, int);
extern "C" void FUN_088399bc(unsigned short *, int, const char *, const char *);
extern "C" void FUN_088d37b8(void *, unsigned short *);

extern "C" void *FUN_088d383c(void *param_1, Item *param_2, unsigned char *param_3)
{
    unsigned short buf[24];
    unsigned short *p = buf;
    FUN_08882764();
    if ((param_3[1] & 1) == 0 || DAT_08beecb1 != 0) {
        char *base = param_2->get_names();
        FUN_088399bc(buf, 17, DAT_08bb4318, base + (param_3[0] - 1) * 17);
    } else {
        p = FUN_08970dbc(DAT_08be85fc, 450);
    }
    FUN_088d37b8(param_1, p);
    return param_1;
}

// Near-miss note: GCC folds identical branch arms; 50/55 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
