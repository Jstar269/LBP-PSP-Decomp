/* LBPMATCH module=EBOOT.BIN start=0x089595fc end=0x0895968c symbol=FUN_089595fc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern void FUN_08afbe68(void *, int);
extern void FUN_0895d1c4(void *, int);
struct S {
    char pad[81 - 0];
    unsigned char started;
    char pad1[6];
    void *h;
    unsigned char flag92;
};
extern void *DAT_08befb88;
void FUN_089595fc(struct S *s)
{
    void *p;
    *((unsigned char *)s + 224) = 0;
    if (!s->started) {
        if (s->flag92) {
            if (!DAT_08befb88)
                return;
            FUN_08afbe68(DAT_08befb88, 1);
            p = ((void **)((char *)s + 88))[0];
            s->started = 1;
            if (p)
                FUN_0895d1c4(p, 1);
        } else {
            p = ((void **)((char *)s + 88))[0];
            s->started = 1;
            if (p)
                FUN_0895d1c4(p, 1);
        }
    }
}
