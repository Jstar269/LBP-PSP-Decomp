/* LBPMATCH module=EBOOT.BIN start=0x088f6974 end=0x088f6a10 symbol=FUN_088f6974 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Page {
    int f0;
    int kind;
    int pad[6];
    unsigned char f20;
    unsigned char pad21[3];
    int f24;
};

extern "C" void *DAT_08bf03b8;
extern "C" bool FUN_0896f3c0(void *);
extern "C" int FUN_0896f7d0(void *);

extern "C" bool FUN_088f6974(Page *param_1)
{
    if (param_1->f20 != 0) {
        if (param_1->kind == 6) {
            if (FUN_0896f7d0(DAT_08bf03b8) == param_1->f24) {
                return false;
            }
        }
        return true;
    }
    if (!FUN_0896f3c0(DAT_08bf03b8)) {
        return false;
    }
    switch (FUN_0896f7d0(DAT_08bf03b8)) {
    case 0:
    case 3:
    case 7:
    case 10:
    case 15:
        return false;
    default:
        return true;
    }
}

// Near-miss note: Global lui hoisting across if/else and switch bitmask order differ.
// near miss from lane oc-lbp-20261121 (unreviewed)
