/* LBPMATCH module=EBOOT.BIN start=0x089484e4 end=0x08948580 symbol=FUN_089484e4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern void FUN_08946fd0(void *);
extern void FUN_0894a488(void *);
extern void *FUN_08b9eb88(void *, int, int);
extern char DAT_08bb7630;
struct S {
    void *vt;
    char buf[1536];
    int f1540;
    char sub1[8];
    int f1552;
    char sub2[148];
    int f1704, f1708;
    char b1712, b1713;
    char pad2[2];
    char k1[32];
    char k2[32];
    unsigned char flag;
};
extern struct S *DAT_08bedb1c;
void *FUN_089484e4(struct S *s)
{
    s->vt = &DAT_08bb7630;
    s->f1540 = 0;
    DAT_08bedb1c = s;
    FUN_08946fd0(s->sub1);
    s->f1552 = 0;
    FUN_0894a488(s->sub2);
    s->flag = 1;
    s->f1704 = 0;
    s->f1708 = 0;
    FUN_08b9eb88(s->buf, 0, 1536);
    s->flag = 1;
    s->b1712 = 0;
    s->b1713 = 0;
    FUN_08b9eb88(s->k1, 0, 32);
    return FUN_08b9eb88(s->k2, 0, 32);
}
