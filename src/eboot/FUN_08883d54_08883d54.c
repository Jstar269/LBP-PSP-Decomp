/* LBPMATCH module=EBOOT.BIN start=0x08883d54 end=0x08883e48 symbol=FUN_08883d54 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Obj {
    void (**vt)(struct Obj *);
};

extern unsigned char DAT_08bee04c[3];
extern int DAT_08bedfac;
extern struct Obj *DAT_08bedfb4;
extern void *DAT_08bee030;
extern unsigned char DAT_08bee059;

extern void FUN_088829e8(void) __attribute__((weak));
extern void *FUN_08b9eb88(void *, int, unsigned);
extern int FUN_08849ff0(struct Obj *);
extern void FUN_08baa11c(int);
extern void FUN_08883ba0(void);
extern void FUN_0888b5bc(void *);
extern void FUN_088824d0(void);
extern void FUN_0888b794(void *);

void FUN_08883d54(void)
{
    FUN_08b9eb88(DAT_08bee04c, 0, 3);
    if ((unsigned)(DAT_08bedfac - 1) < 3) {
        if (DAT_08bedfb4 != 0 && FUN_088829e8 && FUN_08849ff0(DAT_08bedfb4) != 0) {
            if (DAT_08bedfb4 != 0) {
                DAT_08bedfb4->vt[12](DAT_08bedfb4);
            }
        }
        DAT_08bedfac = 4;
    }
    while (DAT_08bedfac != 0) {
        FUN_08baa11c(16);
        FUN_08883ba0();
        FUN_0888b5bc(DAT_08bee030);
    }
    FUN_088824d0();
    if (DAT_08bee059 != 0) {
        FUN_0888b794(DAT_08bee030);
    }
}
