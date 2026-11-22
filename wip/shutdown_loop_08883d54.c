/* LBPMATCH module=EBOOT.BIN start=0x08883d54 end=0x08883e48 symbol=FUN_08883d54 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern void FUN_088829e8(void) __attribute__((weak));
extern int FUN_08849ff0(void *);
extern void FUN_08b9eb88(void *, int, int);
extern void FUN_08baa11c(void);
extern void FUN_08883ba0(void);
extern void FUN_0888b5bc(void *);
extern void FUN_088824d0(int);
extern void FUN_0888b794(void *);
struct VT { void *s[16]; };
struct G { struct VT *vt; };
extern struct G *DAT_08bedfb4;
extern int DAT_08bedfac;
extern char DAT_08bee04c[];
extern void *DAT_08bee030;
extern unsigned char DAT_08bee059;
void FUN_08883d54(void)
{
    FUN_08b9eb88(DAT_08bee04c, 0, 3);
    if ((unsigned)(DAT_08bedfac - 1) < 3) {
        if (DAT_08bedfb4 && FUN_088829e8 && FUN_08849ff0(DAT_08bedfb4) && DAT_08bedfb4)
            ((void (*)(struct G *))DAT_08bedfb4->vt->s[12])(DAT_08bedfb4);
        DAT_08bedfac = 4;
    }
    while (DAT_08bedfac != 0) {
        FUN_08baa11c();
        FUN_08883ba0();
        FUN_0888b5bc(DAT_08bee030);
    }
    FUN_088824d0(16);
    if (DAT_08bee059)
        FUN_0888b794(DAT_08bee030);
}
// NEAR 2 words: only the delay slots differ: retail 'bnez v0,loop; li a0,16; jal FUN_088824d0; nop', ours 'bnez v0,loop; nop; jal; li a0,16' (reorg does not fill the loop branch from the fall-through).
// prototype style (void vs ()), for(;;)/do-while loop shapes tried (those cost 28 words).
// near miss from lane api-lbp-a-20261121 (unreviewed)
