/* LBPMATCH module=EBOOT.BIN start=0x08875e88 end=0x08875f1c symbol=FUN_08875e88 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern void *FUN_08a9e838(int);
extern void FUN_08873608(void *, void *, void *, int);
extern void FUN_0887367c(void *, void *);

struct Obj {
    unsigned char pad[40];
    void *f28;
    int f2c;
};

void **FUN_08875e88(void **out, struct Obj *self, void *param_3)
{
    void *p;
    if (self->f28 != 0) {
        p = FUN_08a9e838(68);
        FUN_08873608(p, param_3, self->f28, self->f2c);
        *out = p;
    } else {
        p = FUN_08a9e838(68);
        FUN_0887367c(p, param_3);
        *out = p;
    }
    return out;
}
