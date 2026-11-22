/* LBPMATCH module=EBOOT.BIN start=0x088c601c end=0x088c60b4 symbol=FUN_088c601c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Inner {
    unsigned char pad1[0x1c];
    int f1c;
    unsigned char pad2[0x240 - 0x20];
    int f240;
};

struct Msg {
    int pad[3];
    struct Inner inner;
};

struct Obj {
    unsigned char pad[0x8c];
    unsigned int flags;
};

void FUN_088c601c(struct Msg **param_1, struct Obj *param_2)
{
    struct Inner *in = &(*param_1)->inner;
    switch (in->f1c) {
    case 0:
        param_2->flags |= 4;
        break;
    case 1:
        param_2->flags |= 8;
        break;
    case 2:
        param_2->flags |= 0x10;
        break;
    }
    switch (in->f240) {
    case 1:
        param_2->flags |= 0x20;
        break;
    case 2:
        param_2->flags |= 0x40;
        break;
    case 3:
        param_2->flags |= 0x80;
        break;
    }
}
