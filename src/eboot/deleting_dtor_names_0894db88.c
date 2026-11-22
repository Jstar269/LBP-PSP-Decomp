/* LBPMATCH module=EBOOT.BIN start=0x0894db88 end=0x0894dc24 symbol=FUN_0894db88 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern void FUN_08a9df6c(void *);
extern void FUN_0894a260(void *);
extern void FUN_08a9e0cc(void *);
extern char DAT_08bb78bc, DAT_08bb78a8, DAT_08bb7aa0, DAT_08bb7ab0;
struct S {
    void *vt0;
    char pad[4];
    void *vt8;
    char pad1[16];
    void *name1;
    void *name2;
    char pad2[1];
    unsigned char b37, b38;
};
void FUN_0894db88(struct S *s)
{
    s->vt8 = &DAT_08bb78bc;
    s->vt0 = &DAT_08bb78a8;
    if (s->name1) {
        FUN_08a9df6c(s->name1);
        s->name1 = 0;
        s->b37 = 0;
    }
    if (s->name2) {
        FUN_08a9df6c(s->name2);
        s->name2 = 0;
        s->b38 = 0;
    }
    s->vt8 = &DAT_08bb7aa0;
    FUN_0894a260(&s->vt8);
    s->vt0 = &DAT_08bb7ab0;
    FUN_0894a260(s);
    FUN_08a9e0cc(s);
}
