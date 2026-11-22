/* LBPMATCH module=EBOOT.BIN start=0x088f0d64 end=0x088f0e00 symbol=FUN_088f0d64 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Slot {
    unsigned char flag;
    unsigned char pad[15];
};

struct Ui {
    unsigned char pad1[0x4c];
    int f4c;
    unsigned char pad2[0x74 - 0x50];
    int f74;
    unsigned char pad3[0x8c - 0x78];
    Slot slots[1];
    unsigned char pad4[0xa4 - 0x9c];
    int fa4;
};

extern "C" void FUN_088f0a34(Ui *, int);
extern "C" void FUN_088ece00(Ui *, unsigned char);

extern "C" int FUN_088f0d64(Ui *param_1, int param_2, int param_3)
{
    if (param_1->slots[param_3].flag != 0) {
        if (param_1->f4c != 4 || param_1->f74 != param_3) {
            FUN_088ece00(param_1, 0);
            return 0;
        }
    } else if (param_1->f4c != 4) {
        FUN_088f0a34(param_1, param_3);
    }
    FUN_088ece00(param_1, 1);
    if (param_2 != param_1->fa4) {
        param_1->fa4 = param_2;
    }
    return 1;
}
