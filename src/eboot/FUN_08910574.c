/* LBPMATCH module=EBOOT.BIN start=0x08910574 end=0x08910604 symbol=FUN_08910574 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18b, GPT-6); exact match verified; names and types await human review. */
typedef unsigned int uint;
struct Item { void **vtable; int mode; };
struct Owner { uint pad0; struct Item *current; uint pad1; struct Item *items[6]; char pad2[76]; int previous; };
void FUN_08910574(struct Owner *self, int mode)
{
    struct Item *old = self->current;
    if (old != 0) {
        if (old->mode == mode)
            return;
        ((void (*)(struct Item *))old->vtable[4])(old);
        self->previous = self->current->mode;
    }
    struct Item *current = self->items[mode];
    self->current = current;
    if (current == 0)
        return;
    ((void (*)(struct Item *))current->vtable[2])(current);
}
