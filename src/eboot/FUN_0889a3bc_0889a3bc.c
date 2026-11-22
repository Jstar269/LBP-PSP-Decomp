/* LBPMATCH module=EBOOT.BIN start=0x0889a3bc end=0x0889a450 symbol=FUN_0889a3bc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern void *DAT_08bb14a0[];
extern char DAT_08bb1518[];

struct Obj {
    void **vt;
    int f4;
    unsigned char b8;
    unsigned char b9;
    unsigned char b10;
    unsigned char b11;
    int f0c;
    unsigned char b16;
    unsigned char pad[3];
};

extern struct Obj *DAT_08bee22c;
extern int *DAT_08bee228;

extern void *FUN_08a9e838(int);
extern int FUN_08baa24c(char *, int, int, int);

struct Obj *FUN_0889a3bc(void)
{
    if (DAT_08bee22c == 0) {
        struct Obj *p = (struct Obj *)FUN_08a9e838(20);
        int *m;
        p->vt = DAT_08bb14a0;
        p->b8 = 0;
        p->b9 = 0;
        p->b10 = 0;
        p->b11 = 0;
        p->b16 = 0;
        DAT_08bee22c = p;
        m = (int *)FUN_08a9e838(4);
        int id = FUN_08baa24c(DAT_08bb1518, 0, 0, 0);
        DAT_08bee228 = m;
        *m = id;
    }
    return DAT_08bee22c;
}
