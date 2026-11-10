/* LBPMATCH module=EBOOT.BIN start=0x088b5a90 end=0x088b5c24 symbol=FUN_088b5a90 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp21h, Claude Opus 5.5); matching idiom: the clear() call takes the tree as (char *)self + 20 rather than &self->tree_14, so the address is recomputed for the last erase loop; exact match verified; names and types await human review. */
typedef unsigned int uint;
typedef struct NodeBase NodeBase;
struct NodeBase { int color; NodeBase *parent; NodeBase *left; NodeBase *right; };
typedef struct Node { NodeBase base; uint key; void *ref; } Node;
typedef struct Tree { uint compare; NodeBase header; uint count; } Tree;
typedef struct Object {
    void *vtable;
    unsigned char pad_04[16];
    Tree tree_14;
    Tree tree_2c;
    Tree tree_44;
} Object;
extern char DAT_08bb3348[];
extern char *DAT_08be84ac;
extern void FUN_0883c3b4(Tree *);
extern void FUN_0883c080(Tree *, NodeBase *);
extern void FUN_088050b8(void *);
extern void FUN_08ad3088(void *);
extern void FUN_08adbc24(void *, void *, uint);
extern void FUN_08ad2810(void *);
extern void FUN_088b2df4(Object *);
extern void FUN_08a9e0cc(void *);

static inline void tree_destroy(Tree *self)
{
    Node *node = (Node *)self->header.parent;
    while (node != 0) {
        FUN_0883c080(self, node->base.right);
        Node *next = (Node *)node->base.left;
        if (node->ref != 0)
            FUN_088050b8(node->ref);
        char *heap = DAT_08be84ac;
        FUN_08ad3088(heap + 32);
        FUN_08adbc24(heap, node, 24);
        FUN_08ad2810(heap + 32);
        node = next;
    }
}

void FUN_088b5a90(Object *self)
{
    self->vtable = DAT_08bb3348;
    FUN_0883c3b4((Tree *)((char *)self + 20));
    tree_destroy(&self->tree_44);
    tree_destroy(&self->tree_2c);
    tree_destroy(&self->tree_14);
    FUN_088b2df4(self);
    FUN_08a9e0cc(self);
}
