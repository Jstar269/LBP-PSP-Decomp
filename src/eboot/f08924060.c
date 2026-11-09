/* LBPMATCH module=EBOOT.BIN start=0x08924060 end=0x08924100 symbol=FUN_08924060 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18c, GPT-6); exact match verified; names and types await human review. */

typedef struct RefObject {
    void **vtable;
    int references;
} RefObject;

extern char DAT_08bb6b08[];
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_08923d84(void *);
extern void FUN_0894a494(void *);
extern char DAT_08bef8f4;
extern char *DAT_08bef76c;

void FUN_08924060(void **param_1)
{
    RefObject *object;
    int references;

    object = (RefObject *)param_1[0x2a];
    *param_1 = DAT_08bb6b08;
    if (object != 0) {
        FUN_08ad3088(&DAT_08bef8f4);
        references = object->references - 1;
        object->references = references;
        FUN_08ad2810(&DAT_08bef8f4);
        if (references == 0) {
            ((void (*)(RefObject *))object->vtable[1])(object);
        }
    }
    FUN_08923d84((char *)param_1 + 0xa0);
    FUN_0894a494((char *)param_1 + 8);
    DAT_08bef76c = 0;
}
