/* LBPMATCH module=EBOOT.BIN start=0x08923c70 end=0x08923d10 symbol=FUN_08923c70 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18c, GPT-6); exact match verified; names and types await human review. */

typedef struct RefObject {
    void **vtable;
    int references;
} RefObject;

typedef struct App {
    void **vtable;
    int status;
    unsigned char pad08[0xa0];
    RefObject *fielda8;
    int flagac;
} App;

extern void FUN_089239d0(void *);
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern char DAT_08bef8f4;
extern App *FUN_089ecaa8(void);
extern int FUN_089eb6a8(App *);

int FUN_08923c70(App *param_1)
{
    RefObject *object;
    int references;

    FUN_089239d0(param_1->fielda8);
    object = param_1->fielda8;
    param_1->fielda8 = 0;
    if (object != 0) {
        FUN_08ad3088(&DAT_08bef8f4);
        references = object->references - 1;
        object->references = references;
        FUN_08ad2810(&DAT_08bef8f4);
        if (references == 0) {
            ((void (*)(RefObject *))object->vtable[1])(object);
        }
    }
    param_1->status = param_1->flagac != 0 ? 6 : 0;
    return FUN_089eb6a8(FUN_089ecaa8());
}
