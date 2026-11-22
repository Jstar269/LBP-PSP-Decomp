/* LBPMATCH module=EBOOT.BIN start=0x08943fd8 end=0x08944068 symbol=FUN_08943fd8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_08b1a2e4(int, int, int, int, int *);
struct S { char pad[392]; char lock[1]; };
extern struct S *DAT_08bedb18;
unsigned char FUN_08943fd8(int unused, int *p1, int *p2, int v, int *p3)
{
    int r = 0;
    char *lock = DAT_08bedb18->lock;
    FUN_08ad3088(lock);
    FUN_08b1a2e4(*p1, *p2, v, *p3, &r);
    FUN_08ad2810(lock);
    return r == 0x1b8;
}
