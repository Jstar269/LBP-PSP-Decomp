/* LBPMATCH module=EBOOT.BIN start=0x0893773c end=0x089377f0 symbol=FUN_0893773c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_089374b4(void *, void *);
struct X { char pad[52]; int a; int b; unsigned short flags; };
struct P { struct X *x; };
struct S1 { char pad[1556]; char lock[128]; char pad2[96]; unsigned char flag; };
struct S2 { char pad[100]; char lock[264]; char sub[1]; };
extern struct S1 *DAT_08bedb1c;
extern struct S2 *DAT_08bedb14;
int FUN_0893773c(struct P *p)
{
    struct S1 *s1 = DAT_08bedb1c;
    unsigned char fl;
    int r;
    FUN_08ad3088(s1->lock);
    fl = s1->flag;
    FUN_08ad2810(s1->lock);
    r = 0;
    if (fl || (p->x->flags & 2)) {
        struct S2 *s2 = DAT_08bedb14;
        struct X *x;
        FUN_08ad3088(s2->lock);
        x = p->x;
        x->a = 0;
        x->flags |= 0x40;
        x->b = 0;
        FUN_089374b4(s2->sub, p);
        FUN_08ad2810(s2->lock);
        r = 1;
    }
    return r;
}
