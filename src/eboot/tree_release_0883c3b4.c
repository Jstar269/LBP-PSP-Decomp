/* LBPMATCH module=EBOOT.BIN start=0x0883c3b4 end=0x0883c468 symbol=FUN_0883c3b4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp16a, GPT-6); exact match verified; names and types await human review. */
typedef unsigned int uint;
typedef struct NodeBase NodeBase;
struct NodeBase { int color; NodeBase *parent; NodeBase *left; NodeBase *right; };
typedef struct Node { NodeBase base; uint key; void *ref; } Node;
typedef struct Tree { uint compare; NodeBase header; uint count; } Tree;
extern char *DAT_08be84ac;
extern void FUN_0883c080(Tree *, NodeBase *);
extern void FUN_088050b8(void *);
extern void FUN_08ad3088(void *);
extern void FUN_08adbc24(void *, void *, uint);
extern void FUN_08ad2810(void *);
void FUN_0883c3b4(Tree *self)
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
    NodeBase *end = &self->header;
    self->header.left = end;
    self->header.parent = 0;
    self->header.right = end;
    self->count = 0;
}
