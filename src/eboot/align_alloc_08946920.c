/* LBPMATCH module=EBOOT.BIN start=0x08946920 end=0x089469bc symbol=FUN_08946920 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_0892cf3c(void *);
struct S { char pad[204]; char lock[1]; char pad1[159]; int n; unsigned char ready; };
extern struct S *DAT_08bedb18;
unsigned FUN_08946920(unsigned x)
{
    struct S *s = DAT_08bedb18;
    int n;
    unsigned u;
    FUN_08ad3088(s->lock);
    if (!s->ready)
        FUN_0892cf3c(s);
    n = s->n;
    FUN_08ad2810(s->lock);
    u = n << 10;
    if (x % u != 0)
        x = ((x + u) / u) * u;
    return x;
}
