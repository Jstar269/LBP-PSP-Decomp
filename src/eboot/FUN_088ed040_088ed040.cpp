/* LBPMATCH module=EBOOT.BIN start=0x088ed040 end=0x088ed0d0 symbol=FUN_088ed040 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Ui {
    unsigned char pad[0xd0];
    unsigned int pad_bits : 22;
    bool b22 : 1;
    unsigned int rest : 9;
};

extern "C" void *DAT_08bf03b8;
extern "C" int FUN_0896f7d0(void *);
extern "C" void FUN_088eca74(Ui *, unsigned char, int);
extern "C" void FUN_088e68a0(Ui *, unsigned char, int);

extern "C" void FUN_088ed040(Ui *param_1, unsigned char param_2)
{
    if (param_2 != 0) {
        if (FUN_0896f7d0(DAT_08bf03b8) == 0x19) {
            if (param_1->b22) {
                FUN_088eca74(param_1, 1, 0x330);
            } else {
                FUN_088eca74(param_1, 1, 0x32d);
            }
            return;
        }
        FUN_088e68a0(param_1, 1, 0x15);
    } else {
        FUN_088e68a0(param_1, 0, 0x19);
        FUN_088e68a0(param_1, 0, 0x15);
    }
}
