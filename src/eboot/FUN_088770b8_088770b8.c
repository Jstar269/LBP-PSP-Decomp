/* LBPMATCH module=EBOOT.BIN start=0x088770b8 end=0x08877148 symbol=FUN_088770b8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bb0738;

extern void FUN_0894a258(void *);
extern int FUN_08ba0e8c(char *);
extern void *FUN_08a9e824(int);
extern void FUN_08889048(void *, char *, int, int *);

struct Obj {
    void *vt;
    int f4;
    void *buf;
    int len;
};

void FUN_088770b8(struct Obj *self, char *param_2)
{
    int out_len;
    int n;
    void *buf;

    FUN_0894a258(self);
    self->f4 = 1;
    self->vt = &DAT_08bb0738;
    n = FUN_08ba0e8c(param_2) + 1;
    buf = FUN_08a9e824(n * 2);
    FUN_08889048(buf, param_2, n, &out_len);
    self->buf = buf;
    self->len = out_len - 1;
}
