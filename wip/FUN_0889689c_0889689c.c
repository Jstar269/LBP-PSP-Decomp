/* LBPMATCH module=EBOOT.BIN start=0x0889689c end=0x08896944 symbol=FUN_0889689c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern void *FUN_08896160(void *, void *);
extern void FUN_08879e80(void *, void *, int);

struct Val {
    int f0;
    unsigned char flag;
    unsigned char pad[3];
    int state;
};

struct Node {
    int pad[9];
    struct Val val;
};

struct Tree {
    int pad;
    struct Node header;
};

struct Obj {
    int f0;
    int f4;
    int f8;
    void *lru;
    struct Tree tree;
};

int FUN_0889689c(struct Obj *self, void *key, unsigned int flags)
{
    struct Node *n = (struct Node *)FUN_08896160(&self->tree, key);
    int ret = 0;

    if (n != &self->tree.header) {
        struct Val *v = &n->val;
        if (flags & 1) {
            v->state = 0;
        } else if (flags & 2) {
            v->state = 1;
        }
        if (flags & 4) {
            v->flag = 1;
        }
        if (self->lru != 0) {
            FUN_08879e80(self->lru, key, 1);
            return 1;
        }
        return 1;
    }
    return 0;
}
// near miss from lane oc-lbp-20261121 (unreviewed)
