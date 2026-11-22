/* LBPMATCH module=EBOOT.BIN start=0x0887acd0 end=0x0887adb4 symbol=FUN_0887acd0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Heap {
    unsigned char pad[32];
    int lock;
};
extern struct Heap *DAT_08be84ac;

extern void FUN_0887ab78(void *);
extern struct Node *FUN_08b89838(struct Node *);
extern struct Node *FUN_08b89b70(struct Node *, struct Node *);
extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);
extern void FUN_08adbc24(struct Heap *, void *, int);

struct Node {
    int color;
    struct Node *parent;
    struct Node *left;
    struct Node *right;
};

struct Tree {
    int pad;
    struct Node header;
    int count;
};

void FUN_0887acd0(struct Tree *self, struct Node *first, struct Node *last)
{
    if (first == self->header.left && last == &self->header) {
        FUN_0887ab78(self);
    } else {
        while (first != last) {
            struct Node *next = FUN_08b89838(first);
            struct Node *y = FUN_08b89b70(first, &self->header);
            {
                struct Heap *h = DAT_08be84ac;
                FUN_08ad3088(&h->lock);
                FUN_08adbc24(h, y, 40);
                FUN_08ad2810(&h->lock);
            }
            self->count--;
            first = next;
        }
    }
}
