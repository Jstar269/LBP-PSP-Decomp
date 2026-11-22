/* LBPMATCH module=EBOOT.BIN start=0x088e66e8 end=0x088e67c4 symbol=FUN_088e66e8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Bus {
    unsigned char pad[0x8c];
    unsigned char f8c;
};

extern "C" Bus *DAT_08bf03b8;
extern "C" bool FUN_0896f3c0(Bus *);
extern "C" int FUN_0896f7d0(Bus *);
extern "C" void FUN_0896ee90(Bus *, int, int);

extern "C" void FUN_088e66e8(void)
{
    for (;;) {
        if (!FUN_0896f3c0(DAT_08bf03b8)) {
            return;
        }
        int id = FUN_0896f7d0(DAT_08bf03b8);
        switch (id) {
        case 0x15:
        case 0x16:
        case 0x18:
        case 0x19:
        case 0x1a:
        case 0x1b:
        case 0x1c:
        case 0x1d:
        case 0x1e:
        case 0x1f:
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x34:
        case 0x35:
        case 0x36:
        case 0x37:
        case 0x39:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x45:
            FUN_0896ee90(DAT_08bf03b8, 1, id);
            DAT_08bf03b8->f8c = 1;
            break;
        default:
            return;
        }
    }
}
