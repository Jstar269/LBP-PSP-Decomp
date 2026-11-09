/* LBPMATCH module=EBOOT.BIN start=0x0889482c end=0x08894908 symbol=FUN_0889482c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17h, GPT-6.1 Terra); exact match verified; names and types await human review. */
struct Node { char pad[8]; struct Node *next; struct Node *child; int tail[10]; };
struct Tree { char pad[8]; struct Node *root; };
struct Pool { char pad[32]; char lock[1]; };
struct Inner { char pad[0x10]; struct Tree tree; char pad2[0xc]; void *buf; };
struct Outer { void *vt; int pad[2]; struct Inner *inner; struct Tree tree; };
extern void *DAT_08bb1060[];
extern struct Pool *DAT_08be84ac;
extern void FUN_08a9e0c4(void *);
extern void FUN_08a9e0cc(void *);
extern void FUN_08894614(struct Tree *, struct Node *);
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_08adbc24(struct Pool *, void *, int);
extern void FUN_0889333c(struct Tree *, struct Node *);
void FUN_0889482c(struct Outer *self) {
    struct Inner *in = self->inner;
    self->vt = DAT_08bb1060;
    if (in != 0) {
        struct Tree *tree = &in->tree;
        if (in->buf != 0) FUN_08a9e0c4(in->buf);
        struct Node *node = tree->root;
        while (node != 0) {
            FUN_08894614(tree, node->child);
            struct Node *next = node->next;
            struct Pool *pool = DAT_08be84ac;
            FUN_08ad3088(pool->lock);
            FUN_08adbc24(pool, node, sizeof(struct Node));
            FUN_08ad2810(pool->lock);
            node = next;
        }
        FUN_08a9e0cc(in);
    }
    struct Tree *tree = &self->tree;
    FUN_0889333c(tree, tree->root);
    FUN_08a9e0cc(self);
}

