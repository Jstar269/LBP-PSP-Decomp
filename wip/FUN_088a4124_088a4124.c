/* LBPMATCH module=EBOOT.BIN start=0x088a4124 end=0x088a4220 symbol=FUN_088a4124 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern void *FUN_0889d194(void);
extern int FUN_0889d08c(void *);
extern void FUN_088a3314(void *);

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
    unsigned char pad1[0xd0];
    int f0d0;
    unsigned char pad2[0x110 - 0xd4];
    void *f110;
    struct Sub sub;
    unsigned char pad3[0x138 - 0x12c];
    int f138;
    int f13c;
    int f140;
    int f144;
};

void FUN_088a4124(struct Obj *self)
{
    struct Sub *sub = &self->sub;
    int x, y, idx;
    struct Entry *e;

    if (self->f138 != 2) {
        return;
    }
    if (self->f13c == 2) {
        self->f110 = &self->f0d0;
    }
    self->f13c = 5;
    x = self->f140;
    y = self->f144;
    idx = FUN_0889d08c(FUN_0889d194());
    e = &sub->entries[idx];
    if (e->a == 5 && x == e->b && y == e->c) {
        FUN_088a3314(self);
        return;
    }
    e->a = 5;
    e->c = y;
    e->b = x;
    sub->vt[4](sub);
    FUN_088a3314(self);
}

// Remaining diffs (61/63 words; 36 differ): entry compare/update control flow and register allocation; also tried C++ Sub::update inline method (64/63 words, 62 differ).
// near miss from lane oc-lbp-20261121 (unreviewed)
