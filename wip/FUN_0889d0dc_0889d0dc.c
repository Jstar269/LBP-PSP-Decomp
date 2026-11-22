/* LBPMATCH module=EBOOT.BIN start=0x0889d0dc end=0x0889d194 symbol=FUN_0889d0dc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Item {
    int f0;
    unsigned int key;
};

struct Obj {
    int f0;
    struct Item *arr[50];
    int count;
};

void FUN_0889d0dc(struct Obj *self, struct Item *item)
{
    int i;
    int swapped;

    for (i = 0; i < self->count; i++) {
        if (item->key == self->arr[i]->key) {
            return;
        }
    }
    self->arr[self->count++] = item;
    do {
        swapped = 0;
        for (i = 0; i < self->count - 1; i++) {
            struct Item *next = self->arr[i + 1];
            if (next->key < self->arr[i]->key) {
                swapped = 1;
                self->arr[i + 1] = self->arr[i];
                self->arr[i] = next;
            }
        }
    } while (swapped);
}
// near miss from lane oc-lbp-20261121 (unreviewed)
