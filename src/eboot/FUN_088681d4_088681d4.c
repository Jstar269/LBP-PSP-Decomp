/* LBPMATCH module=EBOOT.BIN start=0x088681d4 end=0x0886829c symbol=FUN_088681d4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bef8f4;

extern struct Node *FUN_08867bb0(void *, unsigned *);
extern struct Node *FUN_08868028(void *, struct Node *, void *);
extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);

struct Ref {
    void (**vt)(struct Ref *);
    int count;
};

struct Node {
    int color;
    struct Node *parent;
    struct Node *left;
    struct Node *right;
    unsigned key;
    struct Ref *val;
};

struct Tree {
    int pad;
    struct Node header;
};

struct Pair {
    unsigned key;
    struct Ref *val;
};

struct Ref **FUN_088681d4(struct Tree *self, unsigned *key)
{
    struct Node *n = FUN_08867bb0(self, key);
    if (n == &self->header || *key < n->key) {
        struct Pair tmp;
        tmp.key = *key;
        tmp.val = 0;
        n = FUN_08868028(self, n, &tmp);
        if (tmp.val != 0) {
            struct Ref *r = tmp.val;
            int c;
            FUN_08ad3088(&DAT_08bef8f4);
            c = --r->count;
            FUN_08ad2810(&DAT_08bef8f4);
            if (c == 0) {
                r->vt[1](r);
            }
        }
    }
    return &n->val;
}
