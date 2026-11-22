/* LBPMATCH module=EBOOT.BIN start=0x08937564 end=0x08937614 symbol=FUN_08937564 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern void FUN_08931ce0(void *, int);
extern void FUN_089374b4(void *, void *);
struct X {
    char pad[60];
    unsigned short flags;
    char pad2[46];
    int slot[3];
};
struct Pp { struct X *x; };
void FUN_08937564(char *self, struct Pp *p)
{
    struct X *x = p->x;
    unsigned short f = x->flags;
    char *t;
    if (f & 0x10) {
        t = self + 0x1a4;
        if (!(f & 4))
            t = self + 0x19c;
    } else {
        t = self + 0x184;
        if (f & 2) {
            int i;
            for (i = 0; i != 3; i++) {
                if (x->slot[i])
                    FUN_08931ce0(x, i);
            }
            t = self + 0x274;
        }
    }
    FUN_089374b4(t, p);
}
// NEAR 35 (44 words, same length): structure and instruction mix match; only the callee-saved registers differ: retail keeps x=*p in s2 and self in s3, ours self in s2 and x in s3 (all following words shift by that).
// Tried declaration order, cast copy of self, pointer-walk loop, inline helper, inverted if: no change.
// near miss from lane api-lbp-a-20261121 (unreviewed)
