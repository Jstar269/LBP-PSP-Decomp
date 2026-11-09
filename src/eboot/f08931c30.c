/* LBPMATCH module=EBOOT.BIN start=0x08931c30 end=0x08931ce0 symbol=FUN_08931c30 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18d, Codex GPT-6): exact match verified; names and types await human review. */
typedef struct Ref Ref;
typedef struct {
    void *slot0;
    void *slot1;
    void (*slot2)(Ref *);
} RefVTable;
struct Ref {
    RefVTable *vtable;
    int references;
    unsigned char pad[56];
    void (*callback)(Ref **, void *);
    void *callback_arg;
};

extern int DAT_08bef8f4;
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);

void FUN_08931c30(Ref *param_1)
{
    if (param_1->callback != 0) {
        FUN_08ad3088(&DAT_08bef8f4);
        param_1->references = param_1->references + 1;
        FUN_08ad2810(&DAT_08bef8f4);

        Ref *local_20[4];
        local_20[0] = param_1;
        param_1->callback(local_20, param_1->callback_arg);
        Ref *piVar1 = local_20[0];
        if (piVar1 != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            int references = piVar1->references - 1;
            piVar1->references = references;
            FUN_08ad2810(&DAT_08bef8f4);
            if (references == 0) {
                piVar1->vtable->slot2(piVar1);
            }
        }
    }
}
