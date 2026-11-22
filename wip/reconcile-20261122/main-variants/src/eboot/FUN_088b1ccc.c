/* LBPMATCH module=EBOOT.BIN start=0x088b1ccc end=0x088b1d90 symbol=FUN_088b1ccc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17c, Codex GPT-6); exact match verified; names and types await human review. */
typedef struct NodeBase NodeBase;
struct NodeBase { int color; NodeBase *parent; NodeBase *left; NodeBase *right; };
struct Tree { unsigned int compare; NodeBase header; unsigned int count; };
struct LinkedNode {
    void *field_00;
    void *field_04;
    struct LinkedNode *next;
    NodeBase *tree_node;
    int field_10;
    void *resource;
};
struct Object {
    void *vtable;
    unsigned char pad_04[16];
    struct Tree tree;
};
struct Heap { unsigned char pad[32]; unsigned char lock; };
extern unsigned char DAT_08bb30f8[];
extern struct Heap *DAT_08be84ac;
extern void FUN_0883c3b4(struct Tree *);
extern void FUN_0883c080(struct Tree *, NodeBase *);
extern void FUN_088050b8(void *);
extern void FUN_08ad3088(void *);
extern void FUN_08adbc24(void *, void *, unsigned int);
extern void FUN_08ad2810(void *);
extern void FUN_088b2df4(void *);

void FUN_088b1ccc(struct Object *self)
{
    struct Tree *tree = &self->tree;
    self->vtable = DAT_08bb30f8;
    FUN_0883c3b4(tree);
    struct LinkedNode *node = (struct LinkedNode *)tree->header.parent;
    while (node != 0) {
        FUN_0883c080(tree, node->tree_node);
        struct LinkedNode *next = node->next;
        if (node->resource != 0)
            FUN_088050b8(node->resource);
        struct Heap *heap = DAT_08be84ac;
        FUN_08ad3088(&heap->lock);
        FUN_08adbc24(heap, node, 24);
        FUN_08ad2810(&heap->lock);
        node = next;
    }
    return FUN_088b2df4(self);
}

