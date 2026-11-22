/* LBPMATCH module=EBOOT.BIN start=0x08949b18 end=0x08949c00 symbol=FUN_08949b18 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern void FUN_0894a258(void *);
extern void FUN_08b9eb88(void *, int, int);
extern char DAT_08bb76d0, DAT_08bb7710;
struct Sub { int a; int pad; long long b, c; };
struct O {
    void *vt;
    int f4, f8, f12, f16, f20;
    char blk[20];
    unsigned char flag;
    char pad[3];
    int arr[3];
    char t1[128];
    char t2[128];
    char t3[1024];
    char pad5[4];
    char t4[13];
    char pad4[3];
    struct Sub sub;
};
void FUN_08949b18(struct O *o)
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
    FUN_08b9eb88(o->t1, 0, 128);
    FUN_08b9eb88(o->t2, 0, 128);
    FUN_08b9eb88(o->t3, 0, 1024);
    FUN_08b9eb88(o->t4, 0, 13);
    o->vt = &DAT_08bb7710;
    {
        struct Sub *t = &o->sub;
        t->b = 0;
        t->c = 0;
        t->a = 0;
    }
}
// NEAR 2 words: same as wip/ctor_arrays_08949854.c ('li s0,1' before 'addiu v0,v0,30416' in retail). The tail stores match with a Sub{int a; int pad; long long b, c;} at +1360 stored via a hoisted base pointer (t->b = 0; t->c = 0; t->a = 0 gives retail's c-then-b register pair order).
// near miss from lane api-lbp-a-20261121 (unreviewed)
