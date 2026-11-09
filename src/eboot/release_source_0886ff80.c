/* LBPMATCH module=EBOOT.BIN start=0x0886ff80 end=0x08870058 symbol=FUN_0886ff80 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17h, GPT-6.1 Terra); exact match verified; names and types await human review. */
typedef struct Self Self;
typedef struct Source Source;
typedef struct Context { void **vtable; } Context;
typedef struct SelfVTable { void (*slot[6])(); } SelfVTable;
typedef struct SourceVTable { void (*slot[10])(); } SourceVTable;
struct Source { SourceVTable *vtable; };
struct Self { SelfVTable *vtable; int refs; int count; char pad[128]; Source *source; };
extern char DAT_08bef8f4[];
extern void FUN_0886fc68(void *, int, Self *);
extern void FUN_0886f414(Self *);
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_0888256c(void);
void FUN_0886ff80(Context *context, Self *self) {
    if (context->vtable[13] == 0) {
        ((void (*)(Source *, void (*)(void *, int, Self *), Self *, int, int))self->source->vtable->slot[7])(self->source, FUN_0886fc68, self, 0, 1);
        FUN_0886f414(self);
        ((void (*)(Self *))self->vtable->slot[5])(self);
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

