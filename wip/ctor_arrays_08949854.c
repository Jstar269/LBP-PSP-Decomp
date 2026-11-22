/* LBPMATCH module=EBOOT.BIN start=0x08949854 end=0x089498e8 symbol=FUN_08949854 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern void FUN_0894a258(void *);
extern void FUN_08b9eb88(void *, int, int);
extern char DAT_08bb76d0, DAT_08bb76f0;
struct O {
    void *vt;
    int f4, f8, f12, f16, f20;
    char blk[20];
    unsigned char flag;
    char pad[3];
    int arr[3];
    char tail[60];
};
void FUN_08949854(struct O *o)
{
    int i;
    FUN_0894a258(o);
    o->vt = &DAT_08bb76d0;
    o->f4 = 1;
    o->f8 = 0;
    o->f12 = 0;
    o->f16 = 0;
    o->f20 = 0;
    FUN_08b9eb88(o->blk, 0, 20);
    o->flag = 1;
    {
        int *p = o->arr;
        i = 3;
        do {
            i--;
            *p = 0;
            p++;
        } while (i != 0);
    }
    o->vt = &DAT_08bb76f0;
    FUN_08b9eb88(o->tail, 0, 60);
}
// NEAR 2 words: only the order of 'li s0,1' and 'addiu v0,v0,30416' (the vtable address low half) differs after the lui; retail has li first. Source order of f4=1 / vt store did not change it. The 3-word zero loop is retail's 'do { n--; *p = 0; p++; } while (n)' (pointer walk, down counter); a for(i<3) arr[i]=0 loop gives a different loop (20 words).
// near miss from lane api-lbp-a-20261121 (unreviewed)
