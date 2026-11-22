/* LBPMATCH module=EBOOT.BIN start=0x0889d240 end=0x0889d2f4 symbol=FUN_0889d240 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17c, Codex GPT-6); exact match verified; names and types await human review. */
struct Object;
typedef void (*Release)(struct Object *);
typedef void (*Callback)(int *, int);
struct Object {
    Release *vtable;
    int references;
    Callback callback;
    void *field_0c;
    int field_10;
    int field_14;
    unsigned char padding[72];
    int field_60;
    unsigned char tail[12];
};
extern struct Object *FUN_08a9e838(int);
extern void FUN_08949d20(struct Object *, int);
extern void FUN_089487a8(void *, struct Object **);
extern void FUN_08ad3088(void *);
extern int FUN_08ad2810(void *);
extern void FUN_0889fad8(int *, int);
extern void *DAT_08bedb1c;
extern unsigned char DAT_08bef8f4[];

void FUN_0889d240(void)
{
    struct Object *object = FUN_08a9e838(112);
    FUN_08949d20(object, 1);
    object->callback = FUN_0889fad8;
    object->field_0c = (void *)object->field_60;
    object->field_14 = 0;

    struct Object *local_20[4];
    local_20[0] = object;
    FUN_089487a8(DAT_08bedb1c, local_20);
    struct Object *shared = local_20[0];
    if (shared != 0) {
        FUN_08ad3088(DAT_08bef8f4);
        int references = --shared->references;
        FUN_08ad2810(DAT_08bef8f4);
        if (references == 0)
            shared->vtable[1](shared);
    }
}


