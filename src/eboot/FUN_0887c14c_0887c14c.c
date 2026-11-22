/* LBPMATCH module=EBOOT.BIN start=0x0887c14c end=0x0887c1fc symbol=FUN_0887c14c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); matching idiom: volatile on sub->state; exact match verified; names and types await human review. */

extern int FUN_08878e08(void *);
extern void FUN_0889193c(void *, int);

struct Sub {
    int f0;
    volatile int state;
    unsigned char flag;
    unsigned char b9;
    unsigned char pad[2];
};

struct ObjVT {
    void *f0;
    void *f1;
    int (*fn2)(void *);
    void *f3;
    void *f4;
    void *f5;
    void *f6;
    void (*fn7)(void *, int);
};

struct Obj {
    struct ObjVT *vt;
    int f4;
    int f8;
    struct Sub sub;
};

int FUN_0887c14c(void *param_1, struct Obj *param_2, void *param_3)
{
    struct Sub *sub = &param_2->sub;
    int ret;

    sub->state = 3;
    while (FUN_08878e08(param_2) == 0) {
        if (*((unsigned char *)param_2 + 21) != 0) {
            param_2->vt->fn7(param_2, 0);
            FUN_0889193c(param_3, 1000);
        }
    }
    ret = 6;
    if (param_2->vt->fn2(param_2) != 0) {
        ret = sub->flag ? 5 : 4;
    }
    return ret;
}
