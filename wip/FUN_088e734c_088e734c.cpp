/* LBPMATCH module=EBOOT.BIN start=0x088e734c end=0x088e73e0 symbol=FUN_088e734c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct State {
    unsigned char pad[6];
    unsigned char f6;
};

struct Bus {
    unsigned char pad[0x88];
    unsigned char f88;
};

struct Ui {
    unsigned char pad[0xd0];
    bool b0 : 1;
    bool b1 : 1;
    unsigned int pad_bits : 9;
    bool b11 : 1;
    unsigned int rest : 20;
};

extern "C" State *DAT_08bee6a8;
extern "C" unsigned int DAT_08bf03b8;
extern "C" void FUN_088f7414(void);
extern "C" void *FUN_088d7f9c(void);
extern "C" void FUN_088f7950(void *, int, unsigned int);

extern "C" void FUN_088e734c(Ui *param_1)
{
    if (DAT_08bee6a8->f6 != 0) {
        FUN_088f7414();
        if (param_1->b11) {
            FUN_088f7950(FUN_088d7f9c(), 0, 0xffff);
            param_1->b11 = false;
        }
    } else {
        if (param_1->b1 != true) {
            param_1->b1 = true;
            ((Bus *)DAT_08bf03b8)->f88 = 1;
        }
    }
}

// Near-miss note: 2-word scheduling tie; exact length.
// near miss from lane oc-lbp-20261121 (unreviewed)
