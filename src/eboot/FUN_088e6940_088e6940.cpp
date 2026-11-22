/* LBPMATCH module=EBOOT.BIN start=0x088e6940 end=0x088e69d8 symbol=FUN_088e6940 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Bus {
    unsigned char pad[0x8c];
    unsigned char f8c;
};

struct Ui {
    unsigned char pad[0x288];
    int state;
};

extern "C" Bus *DAT_08bf03b8;
extern "C" void *DAT_08be85f8;
extern "C" char DAT_08bb4ac4[];
extern "C" void FUN_0889a8d0(const char *);
extern "C" void *FUN_0889bc4c(void);
extern "C" void FUN_0889bd54(void *);
extern "C" void FUN_08970494(Bus *, int, int);
extern "C" void FUN_08968924(void *, int);
extern "C" void *FUN_0889b768(void);
extern "C" void FUN_0889b9ec(void *);

extern "C" void FUN_088e6940(Ui *param_1)
{
    FUN_0889a8d0(DAT_08bb4ac4);
    if ((unsigned int)(param_1->state - 2) < 3U) {
        FUN_0889bd54(FUN_0889bc4c());
    }
    FUN_08970494(DAT_08bf03b8, 1, 0x48);
    FUN_08968924(DAT_08be85f8, 1);
    DAT_08bf03b8->f8c = 0;
    FUN_0889b9ec(FUN_0889b768());
    param_1->state = 5;
}
