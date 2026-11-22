/* LBPMATCH module=EBOOT.BIN start=0x08878c28 end=0x08878cb8 symbol=FUN_08878c28 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern void FUN_08a9e0c4(void *);
extern void *FUN_08a9e824(unsigned);

struct Obj {
    void *ptr[2];
    unsigned cap[2];
    int idx;
};

void *FUN_08878c28(struct Obj *self, unsigned size)
{
    unsigned cap = self->cap[self->idx];
    if (cap < size) {
        if (self->ptr[self->idx] != 0) {
            FUN_08a9e0c4(self->ptr[self->idx]);
        }
        self->ptr[self->idx] = FUN_08a9e824(size);
        cap = size;
    }
    self->cap[self->idx] = cap;
    return self->ptr[self->idx];
}
