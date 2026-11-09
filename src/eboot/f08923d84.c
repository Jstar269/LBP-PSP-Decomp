/* LBPMATCH module=EBOOT.BIN start=0x08923d84 end=0x08923e50 symbol=FUN_08923d84 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18c, GPT-6); exact match verified; names and types await human review. */

typedef struct RefObject {
    void **vtable;
    int references;
} RefObject;

typedef struct ListNode {
    struct ListNode *next;
    void *previous;
    RefObject *reference;
} ListNode;

typedef struct List {
    ListNode *first;
} List;

extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_08adbc24(void *, void *, int);
extern char DAT_08bef8f4;
extern char *DAT_08be84ac;

void FUN_08923d84(List *param_1)
{
    ListNode *node;
    ListNode *next;
    RefObject *reference;
    int references;
    char *heap;
    char *heap_lock;

    node = param_1->first;
    while (node != (ListNode *)param_1) {
        next = node->next;
        reference = node->reference;
        if (reference != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            references = reference->references - 1;
            reference->references = references;
            FUN_08ad2810(&DAT_08bef8f4);
            if (references == 0) {
                ((void (*)(RefObject *))reference->vtable[1])(reference);
            }
        }
        heap = DAT_08be84ac;
        heap_lock = heap + 32;
        FUN_08ad3088(heap_lock);
        FUN_08adbc24(heap, node, 12);
        FUN_08ad2810(heap_lock);
        node = next;
    }
}
