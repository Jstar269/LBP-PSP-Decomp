/* LBPMATCH module=EBOOT.BIN start=0x08894c1c end=0x08894cbc symbol=FUN_08894c1c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17b, Codex GPT-6); exact match verified; names and types await human review. */

typedef unsigned int uint;
typedef struct Item Item;
struct Item { uint field0; uint field4; Item *next; uint key; };
typedef struct { uint field0; uint field4; Item *head; Item *end0; Item *end1; uint count; } List;

extern char *DAT_08be84ac;
extern void FUN_08894614(List *, uint);
extern void FUN_08ad3088(void *);
extern void FUN_08adbc24(void *, void *, uint);
extern void FUN_08ad2810(void *);

void FUN_08894c1c(List *self)
{
    Item *item = self->head;
    while (item != 0) {
        uint key = item->key;
        FUN_08894614(self, key);
        Item *next = item->next;
        char *heap = DAT_08be84ac;
        FUN_08ad3088(heap + 32);
        FUN_08adbc24(heap, item, 56);
        FUN_08ad2810(heap + 32);
        item = next;
    }
    Item *sentinel = (Item *)&self->field4;
    self->end0 = sentinel;
    self->head = 0;
    self->end1 = sentinel;
    self->count = 0;
}
