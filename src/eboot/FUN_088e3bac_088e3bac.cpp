/* LBPMATCH module=EBOOT.BIN start=0x088e3bac end=0x088e3c44 symbol=FUN_088e3bac lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Pair {
    int a;
    int b;
};

struct Net {
    unsigned char pad[0x6c];
    Pair cur;
};

extern "C" int FUN_088e327c(Net *, int, int, unsigned char);
extern "C" void FUN_088e3540(Net *, Net *, int);

extern "C" void FUN_088e3bac(Net *param_1, Pair *param_2)
{
    int a = param_2->a;
    int b = param_2->b;
    Pair *cur = &param_1->cur;
    unsigned char changed = (a != cur->a || b != cur->b);
    if (FUN_088e327c(param_1, 9, 1, changed) != 0) {
        param_1->cur.b = b;
        param_1->cur.a = a;
        FUN_088e3540(param_1, param_1, 0);
    }
}
