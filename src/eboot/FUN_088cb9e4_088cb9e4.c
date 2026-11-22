/* LBPMATCH module=EBOOT.BIN start=0x088cb9e4 end=0x088cba7c symbol=FUN_088cb9e4 lang=c99 */
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
    unsigned char pad[0x24];
    unsigned int flags;
};

void FUN_088cb9e4(struct Msg **param_1, struct Obj *param_2)
{
    struct Inner *in = &(*param_1)->inner;
    switch (in->f1c) {
    case 0:
        param_2->flags |= 2;
        break;
    case 1:
        param_2->flags |= 4;
        break;
    case 2:
        param_2->flags |= 8;
        break;
    }
    switch (in->f240) {
    case 1:
        param_2->flags |= 0x10;
        break;
    case 2:
        param_2->flags |= 0x20;
        break;
    case 3:
        param_2->flags |= 0x40;
        break;
    }
}
