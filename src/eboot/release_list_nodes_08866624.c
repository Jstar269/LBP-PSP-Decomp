/* LBPMATCH module=EBOOT.BIN start=0x08866624 end=0x088666f0 symbol=FUN_08866624 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17h, GPT-6.1 Terra); exact match verified; names and types await human review. */
typedef struct Ref { void **vtable; int count; } Ref;
typedef struct Node Node;
struct Node { Node *next; int pad[6]; Ref *ref; int tail[3]; };
typedef struct List { Node head; } List;
extern char DAT_08bef8f4[];
extern char *DAT_08be84ac;
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_08adbc24(void *, void *, unsigned int);
void FUN_08866624(List *self) {
    Node *node = self->head.next;
    while (node != &self->head) {
        Ref *ref = node->ref;
        Node *next = node->next;
        if (ref != 0) {
            int count;
            FUN_08ad3088(DAT_08bef8f4);
            count = --ref->count;
            FUN_08ad2810(DAT_08bef8f4);
            if (count == 0) {
                ((void (*)(Ref *))ref->vtable[1])(ref);
            }
        }
        {
            char *heap = DAT_08be84ac;
            char *lock = heap + 32;
            FUN_08ad3088(lock);
            FUN_08adbc24(heap, node, sizeof(Node));
            FUN_08ad2810(lock);
        }
        node = next;
    }
}

