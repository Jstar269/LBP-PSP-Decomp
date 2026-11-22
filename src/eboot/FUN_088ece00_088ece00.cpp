/* LBPMATCH module=EBOOT.BIN start=0x088ece00 end=0x088ece98 symbol=FUN_088ece00 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct State {
    int f0;
    int mode;
};

struct Ui {
    unsigned char pad[0x44];
    int f44;
};

extern "C" void *DAT_08bf03b8;
extern "C" void *DAT_08beed14;
extern "C" int FUN_0896f7d0(void *);
extern "C" State *FUN_088dc4b4(void *);
extern "C" void FUN_088eca74(Ui *, unsigned char, int);
extern "C" void FUN_088e68a0(Ui *, unsigned char, int);

extern "C" void FUN_088ece00(Ui *param_1, unsigned char param_2)
{
    if (param_2 != 0 && (unsigned int)(param_1->f44 - 1) >= 8U) {
        if (FUN_0896f7d0(DAT_08bf03b8) == 0x19 || FUN_088dc4b4(DAT_08beed14)->mode == 2) {
            FUN_088eca74(param_1, 1, 0x2f8);
            return;
        }
        FUN_088e68a0(param_1, 1, 0x14);
        return;
    }
    FUN_088e68a0(param_1, 0, 0x14);
}
