/* LBPMATCH module=EBOOT.BIN start=0x0886829c end=0x08868330 symbol=FUN_0886829c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern void *DAT_08bedba4;
extern void FUN_0887c328(void *, void *);

struct Sub {
    int a;
    int state;
};

struct Obj {
    unsigned char pad1[200];
    struct Sub sub;
    unsigned char pad2[432 - 208];
    int state;
    unsigned char pad3[744 - 436];
    unsigned char flag;
};

int FUN_0886829c(struct Obj *self)
{
    int ret = 0;
    switch (self->state) {
    case 1:
        if (self->flag != 0) {
            struct Sub *sub = &self->sub;
            FUN_0887c328(DAT_08bedba4, sub);
            if (sub->state == 6) {
                self->state = 3;
            } else {
                self->state = 2;
            }
        }
        break;
    case 2:
    case 3:
        ret = 1;
        break;
    default:
        break;
    }
    return ret;
}
