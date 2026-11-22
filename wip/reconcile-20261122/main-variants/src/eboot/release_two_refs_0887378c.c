/* LBPMATCH module=EBOOT.BIN start=0x0887378c end=0x08873864 symbol=FUN_0887378c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17h, GPT-6.1 Terra); exact match verified; names and types await human review. */
typedef struct Ref { void **vtable; int count; } Ref;
typedef struct Object { void **vtable; unsigned char pad[0xd4]; Ref *field_d8; Ref *field_dc; } Object;
extern char DAT_08bef8f4[];
extern void *DAT_08bb0578[];
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_08a9e0cc(void *);
void FUN_0887378c(Object *self) {
    self->vtable = DAT_08bb0578;
    Ref *first = self->field_dc;
    if (first != 0) {
        int count;
        FUN_08ad3088(DAT_08bef8f4);
        count = --first->count;
        FUN_08ad2810(DAT_08bef8f4);
        if (count == 0) {
            ((void (*)(Ref *))first->vtable[1])(first);
        }
    }
    Ref *second = self->field_d8;
    if (second != 0) {
        int count;
        FUN_08ad3088(DAT_08bef8f4);
        count = --second->count;
        FUN_08ad2810(DAT_08bef8f4);
        if (count == 0) {
            ((void (*)(Ref *))second->vtable[1])(second);
        }
    }
    FUN_08a9e0cc(self);
}

