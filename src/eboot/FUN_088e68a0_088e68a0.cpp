/* LBPMATCH module=EBOOT.BIN start=0x088e68a0 end=0x088e6940 symbol=FUN_088e68a0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Bus {
    unsigned char pad[0x8c];
    unsigned char f8c;
};

extern "C" Bus *DAT_08bf03b8;
extern "C" void *DAT_08be85f8;
extern "C" int FUN_0896f7d0(Bus *);
extern "C" void FUN_0896ee90(Bus *, int, int);
extern "C" void FUN_08970494(Bus *, int, int);
extern "C" void FUN_08968924(void *, int);

extern "C" void FUN_088e68a0(void *param_1, unsigned char param_2, int param_3)
{
    (void)param_1;
    if (param_3 != 0x57) {
        if (param_2 != 0) {
            if (FUN_0896f7d0(DAT_08bf03b8) != param_3) {
                FUN_08970494(DAT_08bf03b8, 1, param_3);
                FUN_08968924(DAT_08be85f8, 1);
            }
            DAT_08bf03b8->f8c = 0;
        } else {
            int cur = FUN_0896f7d0(DAT_08bf03b8);
            if (cur == param_3) {
                FUN_0896ee90(DAT_08bf03b8, 1, cur);
            }
        }
    }
}
