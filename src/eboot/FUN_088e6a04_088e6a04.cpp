/* LBPMATCH module=EBOOT.BIN start=0x088e6a04 end=0x088e6aa4 symbol=FUN_088e6a04 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Sub {
    unsigned char pad[0xa8];
    void *fa8;
};

struct Sys;
extern "C" void FUN_08841aac(Sys *, int, int);

struct Sys {
    int pad[2];
    Sub *sub;

    inline void finish(int a, int b)
    {
        FUN_08841aac(this, a, b);
    }
};

struct Ui {
    unsigned char pad[0x68];
    unsigned int mask;
};

extern "C" Sys *DAT_08bedb54;
extern "C" void FUN_08842808(Sys *, unsigned char, unsigned char);
extern "C" int FUN_0891d6e4(void *);
extern "C" int FUN_08841744(Sys *, unsigned char);

extern "C" void FUN_088e6a04(Ui *param_1)
{
    unsigned int i = 1;
    while (param_1->mask != 0) {
        if ((param_1->mask & 1) != 0) {
            FUN_08842808(DAT_08bedb54, 0, (unsigned char)i);
        }
        param_1->mask >>= 1;
        i++;
    }
    Sys *s = DAT_08bedb54;
    int a = FUN_0891d6e4(s->sub->fa8);
    int b = FUN_08841744(DAT_08bedb54, 1);
    s->finish(a, b);
}
