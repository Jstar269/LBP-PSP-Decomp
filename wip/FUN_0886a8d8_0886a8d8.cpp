/* LBPMATCH module=EBOOT.BIN start=0x0886a8d8 end=0x0886a99c symbol=FUN_0886a8d8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Key {
    int f0;
    char *data;
    int num;
};

struct Entry {
    Key k;
    int val;
};

struct Node {
    int color;
    Node *parent;
    Node *left;
    Node *right;
    Key key;
    int val;
};

struct Tree {
    int pad;
    Node header;
};

extern "C" Node *FUN_08868ed8(Tree *, const Key *);
extern "C" int FUN_08b9e714(const void *, const void *, unsigned);
extern "C" void FUN_0884d5f4(Key *, const Key *);
extern "C" Node *FUN_0886a604(Tree *, Node *, Entry *);
extern "C" void FUN_08868d78(Entry *);

inline bool less_key(const Key &a, const Key &b)
{
    int d;
    if (a.num == 0 && b.num == 0) {
        d = FUN_08b9e714(a.data + 12, b.data + 12, 20);
    } else {
        d = a.num - b.num;
    }
    return d < 0;
}

extern "C" int *FUN_0886a8d8(Tree *self, const Key *key)
{
    Node *n = FUN_08868ed8(self, key);
    if (n == &self->header || less_key(*key, n->key)) {
        Entry e;
        FUN_0884d5f4(&e.k, key);
        e.val = 0;
        n = FUN_0886a604(self, n, &e);
        FUN_08868d78(&e);
    }
    return &n->val;
}
// Unresolved compiler pattern (same as docs/LANE-NOTES.md 0x0884c6dc, 0x088506ac, 0x0884cec0):
// retail puts the memcmp arm out of line after the epilogue with `srl v0,v0,31` in its jump delay slot;
// our compiler places the memcmp block inline and uses `bgez` (Os: 34 words, O2: 40 words).
// near miss from lane oc-lbp-20261121 (unreviewed)
