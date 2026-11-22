/* LBPMATCH module=EBOOT.BIN start=0x088755c4 end=0x08875664 symbol=FUN_088755c4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern int FUN_08baa744(int, int, void *, char **, unsigned *);
extern void FUN_08ba10e8(char *, char *, unsigned);
extern int FUN_08878884(void *, char *);

struct Obj {
    int pad[4];
    int f10;
    int f14;
};

int FUN_088755c4(struct Obj *self, void **param_2, void *param_3)
{
    char *ptr;
    unsigned len;
    int ret = 0;

    if (FUN_08baa744(self->f10, self->f14, param_3, &ptr, &len) > 0) {
        char buf[len + 1];
        FUN_08ba10e8(buf, ptr, len);
        buf[len] = 0;
        ret = FUN_08878884(*param_2, buf);
    }
    return ret;
}
// 1 differing word (40/40) at +0x48: ours emits `addiu v0,a2,23` (`(len+1)+7+15`), retail emits `addiu v0,a2,31` (`(len+1)+15+15`).
// Same in lang=c++; __builtin_alloca(len+1) is 37 words off. Likely STACK_BOUNDARY=128 in Sony's GCC vs 64 here (logged in LANE-NOTES.md).
// near miss from lane oc-lbp-20261121 (unreviewed)
