/* LBPMATCH module=EBOOT.BIN start=0x0887f684 end=0x0887f75c symbol=FUN_0887f684 lang=c99 rodata=0x08bb0a70 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern void *FUN_08a9e838(int);
extern void FUN_08858230(void *, const char *, int, void *, void *, int);
extern struct G { int a; char *s; } DAT_08bdbdc4;
struct Arg { int one; int val; };
struct Q { int a; int b; };
struct P { int kind; struct Q q; };
void *FUN_0887f684(struct P *p)
{
    struct Arg a, b;
    void *o;
    struct Q *q = &p->q;
    if (p->kind != 3) {
        char *s = DAT_08bdbdc4.s;
        b.one = 1;
        b.val = q->a;
        o = FUN_08a9e838(704);
        FUN_08858230(o, "downloading", q->b, &b, s, p->kind);
    } else {
        char *s = DAT_08bdbdc4.s;
        a.one = 1;
        a.val = q->a;
        o = FUN_08a9e838(704);
        FUN_08858230(o, "downloading", q->b, &a, s, 3);
    }
    return o;
}
