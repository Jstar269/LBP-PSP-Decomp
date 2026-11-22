/* LBPMATCH module=EBOOT.BIN start=0x08865f30 end=0x08866024 symbol=FUN_08865f30 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bafe78;
extern int DAT_08baec60;
extern int DAT_08bef8f4;

struct Heap {
    unsigned char pad[32];
    int lock;
};
extern struct Heap *DAT_08be84ac;

extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);
extern void FUN_08adbc24(struct Heap *, void *, int);
extern void FUN_0885c9f8(void *);

struct Ref {
    void (**vt)(struct Ref *);
    int count;
};

struct Node {
    struct Node *next;
    struct Node *prev;
    struct Ref *ref;
};

struct Obj {
    void *vt;
    unsigned char pad[204];
    struct Node list;
};

void FUN_08865f30(struct Obj *self)
{
    struct Node *end;
    struct Node *cur;
    self->vt = &DAT_08bafe78;
    end = &self->list;
    cur = self->list.next;
    while (cur != end) {
        struct Ref *r = cur->ref;
        struct Node *next = cur->next;
        if (r != 0) {
            int c;
            FUN_08ad3088(&DAT_08bef8f4);
            c = --r->count;
            FUN_08ad2810(&DAT_08bef8f4);
            if (c == 0) {
                r->vt[2](r);
            }
        }
        {
            struct Heap *h = DAT_08be84ac;
            FUN_08ad3088(&h->lock);
            FUN_08adbc24(h, cur, 12);
            FUN_08ad2810(&h->lock);
        }
        cur = next;
    }
    self->vt = &DAT_08baec60;
    FUN_0885c9f8(self);
}
