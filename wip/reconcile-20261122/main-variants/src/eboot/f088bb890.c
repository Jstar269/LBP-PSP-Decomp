/* LBPMATCH module=EBOOT.BIN start=0x088bb890 end=0x088bb930 symbol=FUN_088bb890 lang=c99 */
/* Hand-written reconstruction from disassembly (lane lbp17d, Codex GPT-6.1): preserves the allocation pointer through the registration call; exact match verified; names and types await human review. */
extern void *FUN_08a9e838(int);
extern void FUN_08a4b630(void *);
extern void FUN_08a4b78c(void *, int, unsigned int, int *);
extern int FUN_08a50fbc(void);
extern void FUN_08a537dc(int, int);
extern void FUN_088b6128(int *);

void FUN_088bb890(int *p)
{
    void *object;
    int result;

    if (p[58] > 0) {
        --p[58];
        object = FUN_08a9e838(0x220);
        FUN_08a4b630(object);
        FUN_08a4b78c(object, p[59 + p[58]],
                     *((unsigned char *)((unsigned int)p[58] +
                                         (unsigned int)p + 0x27c)), p);
        result = FUN_08a50fbc();
        FUN_08a537dc(result, p[59 + p[58]]);
        return;
    }
    FUN_088b6128(p);
}
