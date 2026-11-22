/* LBPMATCH module=EBOOT.BIN start=0x088ae6d4 end=0x088ae78c symbol=FUN_088ae6d4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Node {
    struct Node *next;
    struct Node *prev;
};

struct Obj {
    int f0;
    int pad[5];
    struct Node *arr;
    unsigned short f1c;
    unsigned short count;
};

extern void FUN_088ae22c(struct Obj *);
extern void FUN_088ae560(struct Obj *);
extern void FUN_088aede4(struct Node *, int);
extern void FUN_088adcb8(struct Node *, struct Node *);

void FUN_088ae6d4(struct Obj *self)
{
    unsigned short i;

    FUN_088ae22c(self);
    FUN_088ae560(self);
    for (i = 0; i < self->count; i++) {
        struct Node *arr;
        struct Node *l;
        struct Node *p;
        unsigned int n = 0;

        FUN_088aede4(&self->arr[i], self->f0);
        arr = self->arr;
        l = &arr[i];
        for (p = l->next; p != l; p = p->next) {
            n++;
        }
        if (n < 3) {
            unsigned short last = self->count - 1;
            self->count = last;
            FUN_088adcb8(l, &arr[last]);
        }
    }
}
