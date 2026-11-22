/* LBPMATCH module=EBOOT.BIN start=0x0888a4d0 end=0x0888a574 symbol=FUN_0888a4d0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); matching idiom: volatile on self->f4/f54/b4d8/b4d9; exact match verified; names and types await human review. */

extern int FUN_08baa59c(int *);
extern void FUN_08baa5a4(void);
extern void FUN_08baa5ac(void);
extern void FUN_08baa11c(int);

struct Obj {
    int f0;
    volatile int f4;
    unsigned char pad1[84 - 8];
    volatile int f54;
    unsigned char pad2[1240 - 88];
    volatile unsigned char b4d8;
    volatile unsigned char b4d9;
};

void FUN_0888a4d0(struct Obj *self)
{
    int status;
    if (FUN_08baa59c(&status) == 0 && status == 1 && self->f54 != 0 && self->b4d8 == 0) {
        int n = 120;
        FUN_08baa5a4();
        FUN_08baa5ac();
        self->f4 = 1;
        self->b4d8 = 1;
        self->b4d9 = 1;
        for (;;) {
            if (self->f54 == 0) {
                break;
            }
            if (n == 0) {
                break;
            }
            n--;
            FUN_08baa11c(16);
        }
    }
}
