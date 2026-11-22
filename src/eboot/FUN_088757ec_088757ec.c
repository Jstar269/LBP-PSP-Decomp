/* LBPMATCH module=EBOOT.BIN start=0x088757ec end=0x0887588c symbol=FUN_088757ec lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern void *FUN_08878bfc(void *);
extern void FUN_08878bec(void *);
extern int FUN_08baa4d4(int, void *, int);
extern int FUN_0887d58c(int, int);

struct Sub {
    int pad[4];
    int size;
};

struct Obj {
    unsigned char pad1[12];
    unsigned char b0c;
    unsigned char pad2[11];
    struct Sub sub;
    unsigned char pad3[72 - 44];
    int f48;
    int pad4;
    int f50;
};

void *FUN_088757ec(struct Obj *self, int *param_2)
{
    struct Sub *sub = &self->sub;
    void *buf = FUN_08878bfc(sub);
    int size = sub->size;
    int r;
    void *ret = 0;

    FUN_08878bec(sub);
    r = FUN_08baa4d4(self->f48, buf, size);
    if (r < 0) {
        int e = FUN_0887d58c(r, self->f48);
        self->f50 = e;
        self->b0c = 1;
    } else if (r != 0) {
        *param_2 = r;
        ret = buf;
    }
    return ret;
}
