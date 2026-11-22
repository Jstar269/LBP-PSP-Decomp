/* LBPMATCH module=EBOOT.BIN start=0x0886d99c end=0x0886da74 symbol=FUN_0886d99c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17h, GPT-6.1 Terra); exact match verified; names and types await human review. */
typedef struct Ref { void **vtable; int count; } Ref;
typedef struct Object { unsigned char pad0[12]; unsigned int state; unsigned char pad1[52]; Ref *first; Ref *second; } Object;
extern char DAT_08bef8f4[];
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
void FUN_0886d99c(Object *self) {
    Ref *first = self->first;
    self->first = 0;
    if (first != 0) {
        int count;
        FUN_08ad3088(DAT_08bef8f4);
        count = --first->count;
        FUN_08ad2810(DAT_08bef8f4);
        if (count == 0) {
            ((void (*)(Ref *))first->vtable[1])(first);
        }
    }
    Ref *second = self->second;
    self->second = 0;
    if (second != 0) {
        int count;
        FUN_08ad3088(DAT_08bef8f4);
        count = --second->count;
        FUN_08ad2810(DAT_08bef8f4);
        if (count == 0) {
            ((void (*)(Ref *))second->vtable[1])(second);
        }
    }
    self->state = 3;
}

