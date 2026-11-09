/* LBPMATCH module=EBOOT.BIN start=0x08871d5c end=0x08871e28 symbol=FUN_08871d5c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17h, GPT-6.1 Terra); exact match verified; names and types await human review. */
typedef struct Self Self;
typedef struct Source Source;
typedef void (*Callback)(void *, int, Self *);
typedef struct Context { void **vtable; } Context;
typedef struct SelfVTable { void (*slot[6])(); } SelfVTable;
typedef struct SourceVTable { void (*slot[10])(); } SourceVTable;
struct Source { SourceVTable *vtable; };
struct Self { SelfVTable *vtable; int refs; int count; char pad[124]; Source *source; };
extern char DAT_08bef8f4[];
extern void FUN_088706f4(void *, int, Self *);
extern void FUN_08871a38(Self *);
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_0888256c(void);
void FUN_08871d5c(Context *context, Self *self) {
    if (context->vtable[13] == 0) {
        ((void (*)(Source *, Callback, Self *, int, int))self->source->vtable->slot[7])(self->source, FUN_088706f4, self, 0, 1);
        FUN_08871a38(self);
    } else {
        ((void (*)(Self *))self->vtable->slot[4])(self);
    }
    FUN_08ad3088(DAT_08bef8f4);
    int refs = self->refs - 1;
    --self->count;
    self->refs = refs;
    FUN_08ad2810(DAT_08bef8f4);
    if (refs == 0) {
        ((void (*)(Self *))self->vtable->slot[1])(self);
    }
    FUN_0888256c();
}

