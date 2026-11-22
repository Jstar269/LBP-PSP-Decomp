/* LBPMATCH module=EBOOT.BIN start=0x088d0d50 end=0x088d0e0c symbol=FUN_088d0d50 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Sub {
    unsigned char pad[0x14];
    unsigned short f14;
};

struct Ui {
    unsigned char pad[0x18];
    Sub *sub;
};

struct State {
    int f0;
    int mode;
};

struct Mgr {
    unsigned char pad[0x194];
    int f194;
};

struct Net {
    unsigned char pad[0x24];
    unsigned int f24;
};

extern "C" Mgr *DAT_08beed14;
extern "C" Net *DAT_08beed8c;
extern "C" void *DAT_08bf03b8;
extern "C" Ui *FUN_088d7f9c(void);
extern "C" void FUN_089702b4(void *, int);
extern "C" void FUN_088f71f0(Ui *, unsigned char);
extern "C" State *FUN_088dc4b4(Mgr *);
extern "C" void FUN_088f72d8(Ui *, unsigned char, unsigned int);

extern "C" void FUN_088d0d50(void)
{
    Ui *u = FUN_088d7f9c();
    FUN_089702b4(DAT_08bf03b8, 1);
    FUN_088f71f0(u, 0);
    if (FUN_088dc4b4(DAT_08beed14)->mode != 1 || DAT_08beed14->f194 != 0) {
        Net *n = DAT_08beed8c;
        if (FUN_088dc4b4(DAT_08beed14)->mode != 2 || n->f24 < 2) {
            FUN_088f72d8(u, 1, u->sub->f14);
        }
    }
}
