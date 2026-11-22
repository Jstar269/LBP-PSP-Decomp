/* LBPMATCH module=EBOOT.BIN start=0x088795d4 end=0x088796ac symbol=FUN_088795d4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17h, GPT-6.1 Terra); exact match verified; names and types await human review. */
typedef struct Ref { void **vtable; int count; } Ref;
typedef struct Node { int first; int second; struct Node *next; struct Node *child; int pad[8]; Ref *ref; } Node;
extern char DAT_08bef8f4[];
extern char *DAT_08be84ac;
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_08adbc24(void *, void *, unsigned int);
void FUN_088795d4(void *self, Node *node) {
    while (node != 0) {
        Node *child = node->child;
        FUN_088795d4(self, child);
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

