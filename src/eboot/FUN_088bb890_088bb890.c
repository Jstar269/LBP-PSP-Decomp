/* LBPMATCH module=EBOOT.BIN start=0x088bb890 end=0x088bb930 symbol=FUN_088bb890 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Obj {
    unsigned char pad1[0xe8];
    int count;
    unsigned int arr[100];
    unsigned char flags[100];
};

extern void *FUN_08a9e838(int);
extern void FUN_08a4b630(void *);
extern void FUN_08a4b78c(void *, unsigned int, unsigned char, struct Obj *);
extern void *FUN_08a50fbc(void);
extern void FUN_08a537dc(void *, unsigned int);
extern void FUN_088b6128(struct Obj *);

void FUN_088bb890(struct Obj *self)
{
    if (self->count > 0) {
        void *p;
        self->count--;
        p = FUN_08a9e838(544);
        FUN_08a4b630(p);
        FUN_08a4b78c(p, self->arr[self->count], self->flags[self->count], self);
        FUN_08a537dc(FUN_08a50fbc(), self->arr[self->count]);
        return;
    }
    FUN_088b6128(self);
}
