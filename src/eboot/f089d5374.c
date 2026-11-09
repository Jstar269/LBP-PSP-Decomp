/* LBPMATCH module=EBOOT.BIN start=0x089d5374 end=0x089d54a8 symbol=FUN_089d5374 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp21d, Opus 5.5); exact match verified; names and types await human review. */

typedef struct Page {
    char pad0[12];
    unsigned short index;
} Page;

typedef struct Span {
    int pad0;
    int pad4;
    Page *page;
    int first;
    int last;
    short pad14;
    unsigned short count;
} Span;

typedef struct Link {
    struct Link *next;
    struct Link *prev;
} Link;

typedef struct Item {
    Link link;
    Span span;
} Item;

typedef Item Group;

typedef struct Owner {
    int pad0;
    unsigned int flags;
    char pad8[16];
    char pool[120];
    Link head;
    int current;
    char pad9c[26];
    short per_page;
} Owner;

extern Group *FUN_089d3bd4(Owner *, Item *);
extern int FUN_089d3b80(Owner *, Item *);
extern void FUN_089d502c(Owner *, int, int);
extern void FUN_08b88fc8(void *);
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_08adbc24(void *, void *, int);
extern char *DAT_08be84ac;

void FUN_089d5374(Owner *param_1, Item *param_2)
{
    Span *span = &param_2->span;
    int first;
    short last;
    int delta;
    Group *group;

    void *block;
    char *heap;
    char *heap_lock;

    first = (short)(param_2->span.page->index + 1);
    last = (span->count - 1) / param_1->per_page + first;
    delta = -2 - (last - first);
    group = FUN_089d3bd4(param_1, param_2);
    if (&group->link != &param_1->head) {
        Span *gspan = &group->span;
        if (gspan->first == gspan->last) {
            gspan->first = span->last;
        }
        gspan->last = span->last;
    } else {
        param_1->current = FUN_089d3b80(param_1, param_2);
    }
    block = span->page;
    FUN_08b88fc8(block);
    heap = DAT_08be84ac;
    heap_lock = heap + 32;
    FUN_08ad3088(heap_lock);
    FUN_08adbc24(heap, block, 20);
    FUN_08ad2810(heap_lock);
    span->page = (Page *)param_1->pool;
    span->count = 0;
    FUN_089d502c(param_1, span->last, delta);
    param_1->flags |= 0x400;
}
