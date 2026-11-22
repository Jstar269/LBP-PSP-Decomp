/* LBPMATCH module=EBOOT.BIN start=0x08875c68 end=0x08875cf8 symbol=FUN_08875c68 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int FUN_08baa4ac(int, void *, void *);
extern int FUN_0887d5c8(int, int, void *, void *);
extern int FUN_0887d58c(int, int);

struct Obj {
    unsigned char pad1[12];
    unsigned char b0c;
    unsigned char pad2[7];
    int f14;
    int pad3;
    int f1c;
};

int FUN_08875c68(struct Obj *self, void *param_2, void *param_3)
{
    int r;
    int ret = 1;

    r = FUN_08baa4ac(self->f14, param_2, param_3);
    if (r == (int)0x80410414) {
        r = FUN_0887d5c8(self->f14, 3, param_2, param_3);
    }
    if (r < 0) {
        int e = FUN_0887d58c(r, self->f14);
        self->b0c = 1;
        self->f1c = e;
        ret = 0;
    }
    return ret;
}
