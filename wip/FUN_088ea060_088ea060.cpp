/* LBPMATCH module=EBOOT.BIN start=0x088ea060 end=0x088ea150 symbol=FUN_088ea060 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Ui {
    unsigned char pad[0xcc];
    unsigned short *str;
    unsigned int pad_bits : 14;
    bool b14 : 1;
    unsigned int rest : 17;
};

struct Bus {
    unsigned char pad[0x8c];
    unsigned char f8c;
};

extern "C" Ui *DAT_08beee0c;
extern "C" Bus *DAT_08bf03b8;
extern "C" void *FUN_089ecaa8(void);
extern "C" unsigned short *FUN_089eaae0(void *, void *);
extern "C" void FUN_08a9e0c4(void *);
extern "C" void *FUN_08a9e824(unsigned int);
extern "C" int FUN_0896eebc(Bus *, int, int);
extern "C" void FUN_0896ee90(Bus *, int, int);

extern "C" void FUN_088ea060(void *param_1)
{
    DAT_08beee0c->b14 = true;
    unsigned short *s = FUN_089eaae0(FUN_089ecaa8(), param_1);
    if (s != 0) {
        Ui *u = DAT_08beee0c;
        unsigned short *src = s;
        if (u->str != 0) {
            FUN_08a9e0c4(u->str);
        }
        unsigned short *p = s;
        while (*p != 0) {
            p++;
        }
        unsigned short *dst = (unsigned short *)FUN_08a9e824(((unsigned int)((char *)p - (char *)s) & ~1U) + 2);
        u->str = dst;
        while (*src != 0) {
            *dst++ = *src++;
        }
        *dst = 0;
        FUN_08a9e0c4(s);
    }
    if (FUN_0896eebc(DAT_08bf03b8, 2, 3) != 0) {
        FUN_0896ee90(DAT_08bf03b8, 2, 3);
        DAT_08bf03b8->f8c = 0;
    }
}

// Near-miss note: Branch/regalloc diff on 59/60 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
