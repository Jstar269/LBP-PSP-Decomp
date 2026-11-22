/* LBPMATCH module=EBOOT.BIN start=0x08946920 end=0x089469bc symbol=FUN_08946920 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp21e, Claude Opus 5.5); exact match verified; names and types await human review. */
extern char *DAT_08bedb18;
void FUN_08ad3088(void *m);
void FUN_08ad2810(void *m);
void FUN_0892cf3c(char *p);

unsigned int FUN_08946920(unsigned int size)
{
    char *p = DAT_08bedb18;
    unsigned int blk;
    int n;

    FUN_08ad3088(p + 0xcc);
    if (*(unsigned char *)(p + 0x170) == 0)
        FUN_0892cf3c(p);
    n = *(int *)(p + 0x16c);
    FUN_08ad2810(p + 0xcc);
    blk = n << 10;
    if (size % blk != 0)
        size = ((size + blk) / blk) * blk;
    return size;
}
