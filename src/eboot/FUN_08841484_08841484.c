/* LBPMATCH module=EBOOT.BIN start=0x08841484 end=0x08841554 symbol=FUN_08841484 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17g, Codex GPT-6.1); exact match verified; names and types await human review. */

typedef void (*Destroy_08841484)(void *);
typedef struct Shared_08841484 {
    Destroy_08841484 *vtable;
    int references;
} Shared_08841484;
typedef struct ListNode_08841484 {
    struct ListNode_08841484 *next;
} ListNode_08841484;

extern void FUN_08908628(void *);
extern void FUN_08a9e0cc(void *);
extern char DAT_08bef8f4[];
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_08adbc24(void *, void *, int);
extern void *DAT_08be84a0;
extern unsigned char DAT_08bae0b8;
extern int DAT_08bedb50;

void FUN_08841484(char *param_1) {
    void *resource = *(void **)(param_1 + 0x1c);
    int *listBase;
    Shared_08841484 *shared;
    int references;
    ListNode_08841484 *sentinel;
    ListNode_08841484 *node;
    ListNode_08841484 *next;

    if (resource != 0) {
        FUN_08908628(resource);
        FUN_08a9e0cc(resource);
    }
    listBase = (int *)(param_1 + 4);
    shared = *(Shared_08841484 **)(listBase + 4);
    *(void ***)(param_1 + 0x20) = (void **)&DAT_08bae0b8;
    if (shared != 0) {
        FUN_08ad3088(&DAT_08bef8f4);
        references = shared->references - 1;
        shared->references = references;
        FUN_08ad2810(&DAT_08bef8f4);
        if (references == 0) {
            shared->vtable[2](shared);
        }
    }
    node = *(ListNode_08841484 **)(listBase + 1);
    sentinel = (ListNode_08841484 *)(listBase + 1);
    while (node != sentinel) {
        next = node->next;
        FUN_08adbc24(DAT_08be84a0, node, 0x10);
        node = next;
    }
    DAT_08bedb50 = 0;
}

