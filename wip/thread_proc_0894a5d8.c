/* LBPMATCH module=EBOOT.BIN start=0x0894a5d8 end=0x0894a680 symbol=FUN_0894a5d8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_08ad20a4(void *, void *);
extern void FUN_08ad17dc(int);
struct S {
    char pad0[8];
    char ev[420];
    char lock[148];
    void (*fn)(void);
    void (*fin)(void);
    unsigned char count;
    unsigned char busy;
    unsigned char stop;
};
int FUN_0894a5d8(struct S *s)
{
    char *lock;
    while (lock = s->lock, !s->stop) {
        FUN_08ad3088(lock);
        if (s->count) {
            s->busy = 1;
            FUN_08ad20a4(s->ev, lock);
            s->busy = 0;
        }
        FUN_08ad2810(lock);
        if (!s->stop)
            s->fn();
    }
    if (s->fin)
        s->fin();
    FUN_08ad17dc(0);
    return 0;
}
// NEAR 31 as counted by the matcher (41 vs 42 words; every difference is the one shifted nop): retail does not put the byte store 'sb zero,585(s0)' into the delay slot of 'jal 0x8ad20a4' (jal; nop; sb), ours does. Same evidence as lane api-sonnet-20261121 on 0x088746c4/0x088720b4: retail never fills a branch/call delay slot with sb.
// near miss from lane api-lbp-a-20261121 (unreviewed)
