/* LBPMATCH module=EBOOT.BIN start=0x089226ec end=0x08922798 symbol=FUN_089226ec lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18c, GPT-6); exact match verified; names and types await human review. */

typedef struct RefObject {
    void **vtable;
    int references;
} RefObject;

typedef struct Object {
    unsigned char pad00[8];
    RefObject *field8;
} Object;

extern void FUN_08b88fc8(Object *);
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_08adbc24(void *, void *, int);
extern char DAT_08bef8f4;
extern void *DAT_08be84ac;

void FUN_089226ec(void *param_1, Object *param_2)
{
    RefObject *object;
    int references;
    char *base;
    char *lock;

    FUN_08b88fc8(param_2);
    object = param_2->field8;
    if (object != 0) {
        FUN_08ad3088(&DAT_08bef8f4);
        references = object->references - 1;
        object->references = references;
        FUN_08ad2810(&DAT_08bef8f4);
        if (references == 0) {
            ((void (*)(RefObject *))object->vtable[2])(object);
        }
    }
    base = (char *)DAT_08be84ac;
    lock = base + 32;
    FUN_08ad3088(lock);
    FUN_08adbc24(base, param_2, 12);
    FUN_08ad2810(lock);
}
