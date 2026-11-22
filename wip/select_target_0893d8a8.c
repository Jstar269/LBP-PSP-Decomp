/* LBPMATCH module=EBOOT.BIN start=0x0893d8a8 end=0x0893d97c symbol=FUN_0893d8a8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern void FUN_08931ce0(void *, int);
extern void FUN_089374b4(void *, void *);
struct X {
    char pad[40];
    unsigned size;
    char pad1[8];
    int state;
    int pad2;
    unsigned short flags;
    char pad3[46];
    int slot[3];
};
struct Pp { struct X *x; };
static inline void touch(struct Pp *p)
{
    struct X *x = p->x;
    int i;
    for (i = 0; i != 3; i++) {
        if (x->slot[i])
            FUN_08931ce0(x, i);
    }
}
void FUN_0893d8a8(char *self, struct Pp *p)
{
    struct X *x = p->x;
    unsigned size = x->size;
    char *t;
    if (size == 0) {
        t = self + 372;
    } else {
        unsigned lim = 25;
        if (!(x->flags & 0x10))
            lim = 0;
        if (lim < size && size < 0x100000) {
            t = self + 380;
        } else {
            x->state = 2;
            touch(p);
            t = self + 636;
        }
    }
    FUN_089374b4(t, p);
}
// NEAR 50 (51 of 53 words): logic and block order match (p->x re-read inside the inlined slot loop gives retail's reload); retail keeps self only in s2 and loads size into a0 (so the self+372 add uses s2 in the j delay slot) and copies p to s3 late (bnez delay slot), while ours copies it in the prologue and keeps self in a0 for the delay-slot add; retail also tail-calls the +380 case with its own epilogue copy (2 more words).
// Tried: size cached/not cached, ?: for the limit, inverted if, early return for the +380 case.
// near miss from lane api-lbp-a-20261121 (unreviewed)
