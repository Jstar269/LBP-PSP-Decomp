/* LBPMATCH module=EBOOT.BIN start=0x088d31f0 end=0x088d3298 symbol=FUN_088d31f0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Sys {
    unsigned char pad1[0xcc];
    int lock;
    unsigned char pad2[0x16c - 0xd0];
    int blk;
    unsigned char init;
};

struct Item {
    unsigned char pad[0x6c];
    unsigned int size;
};

extern struct Sys *DAT_08bedb18;
extern void FUN_08ad3088(int *);
extern void FUN_0892cf3c(struct Sys *);
extern void FUN_08ad2810(int *);

void FUN_088d31f0(struct Item *param_1, unsigned int param_2)
{
    struct Sys *s = DAT_08bedb18;
    FUN_08ad3088(&s->lock);
    if (s->init == 0) {
        FUN_0892cf3c(s);
    }
    int blk = s->blk;
    FUN_08ad2810(&s->lock);
    unsigned int sz = (unsigned int)blk << 10;
    unsigned int q = param_2 / sz;
    unsigned int r = param_2 % sz;
    if (r != 0) {
        q++;
    }
    param_1->size += (q * sz) >> 10;
}

// Near-miss note: 2-word scheduling tie; exact length.
// near miss from lane oc-lbp-20261121 (unreviewed)
