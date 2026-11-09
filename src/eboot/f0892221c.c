/* LBPMATCH module=EBOOT.BIN start=0x0892221c end=0x089222cc symbol=FUN_0892221c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18c, GPT-6); exact match verified; names and types await human review. */

typedef struct RefObject {
    void **vtable;
    int references;
} RefObject;

typedef struct Object {
    void **vtable;
    int references;
    unsigned char pad08[0x2cc];
    void (*callback)(void **, void *);
    int pad728;
    void *callback_arg;
} Object;

extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern char DAT_08bef8f4;

void FUN_0892221c(Object *param_1)
{
    void *local_20[4];
    RefObject *object;
    int references;

    if (param_1->callback != 0) {
        FUN_08ad3088(&DAT_08bef8f4);
        param_1->references = param_1->references + 1;
        FUN_08ad2810(&DAT_08bef8f4);
        local_20[0] = param_1;
        param_1->callback(local_20, param_1->callback_arg);
        object = (RefObject *)local_20[0];
        if (object != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            references = object->references - 1;
            object->references = references;
            FUN_08ad2810(&DAT_08bef8f4);
            if (references == 0) {
                ((void (*)(RefObject *))object->vtable[2])(object);
            }
        }
    }
}
