/* LBPMATCH module=EBOOT.BIN start=0x088789c8 end=0x08878ab8 symbol=FUN_088789c8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern void FUN_08b9eb88(void *, int, int);

struct Obj {
    unsigned char pad[12];
    unsigned char hash[20];
};

static inline unsigned char hex_val(char c)
{
    if ((unsigned char)(c - '0') < 10)
        return (unsigned char)(c - '0');
    if ((unsigned char)(c - 'A') < 26)
        return (unsigned char)(c - 55);
    if ((unsigned char)(c - 'a') < 26)
        return (unsigned char)(c - 87);
    return 0xff;
}

void FUN_088789c8(struct Obj *self, const char *s)
{
    int i;
    for (i = 0; i != 20; i++) {
        unsigned char hi = hex_val(s[0]);
        unsigned char lo;
        if (hi > 15) {
            FUN_08b9eb88(self->hash, 0, 20);
            return;
        }
        lo = hex_val(s[1]);
        if (lo > 15) {
            FUN_08b9eb88(self->hash, 0, 20);
            return;
        }
        self->hash[i] = (unsigned char)((hi << 4) | lo);
        s += 2;
    }
}
// 39 differing words (61/60): retail leaves `self->hash[i]` un-strength-reduced (`addu v0,a0,t0; sb v1,12(v0)`)
// and keeps `sll v0,v1,4; andi t1,v0,0xff` before the second hex_val; ours strength-reduces `self->hash[i]` to a pointer bump.
// near miss from lane oc-lbp-20261121 (unreviewed)
