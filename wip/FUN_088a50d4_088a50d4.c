/* LBPMATCH module=EBOOT.BIN start=0x088a50d4 end=0x088a519c symbol=FUN_088a50d4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern void FUN_088a4e20(void *);
extern void *FUN_0889d194(void);
extern int FUN_0889d08c(void *);

struct Entry {
    int a;
    int b;
    int c;
};

struct Sub {
    void (**vt)(struct Sub *);
    int pad[4];
    struct Entry *entries;
};

struct Obj {
    unsigned char pad1[0x114];
    struct Sub sub;
    unsigned char pad2[0x13c - 0x12c];
    int f13c;
    int f140;
    int f144;
};

void FUN_088a50d4(struct Obj *self, int param_2)
{
    struct Sub *sub;
    int y, idx;
    struct Entry *e;

    FUN_088a4e20(self);
    self->f13c = 1;
    sub = &self->sub;
    self->f140 = param_2;
    y = self->f144;
    idx = FUN_0889d08c(FUN_0889d194());
    e = &sub->entries[idx];
    if (e->a != 1 || param_2 != e->b || y != e->c) {
        e->a = 1;
        e->c = y;
        e->b = param_2;
        sub->vt[4](sub);
    }
}

// Remaining diffs (50/50 words; 34 differ): register allocation / sub-entry compare ordering; also tried C++ Sub::update inline method (53/50 words, 49 differ).
// near miss from lane oc-lbp-20261121 (unreviewed)
