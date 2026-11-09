/* LBPMATCH module=EBOOT.BIN start=0x0892160c end=0x089216e8 symbol=FUN_0892160c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18c, GPT-6); exact match verified; names and types await human review. */

typedef void (*Callback)(void *, void *);

typedef struct Object {
    char *vtable;
    int references;
    char *subobject_vtable;
    int pad0c;
    unsigned char field10[24];
    int field28;
    int field2c;
    int field30;
    int field34;
    int field38;
    unsigned short field3c;
    unsigned char pad3e[2];
    int field40;
    int field44;
    Callback callbacks48[3];
    Callback callbacks54[3];
    void *callbacks60[3];
    void *callbacks6c[3];
    void *callbacks78[3];
    int callbacks84[3];
    void *field90;
} Object;

extern char DAT_08bb6a40[];
extern char DAT_08bb6a54[];
extern char DAT_08bb6a78[];
extern void FUN_0894a258(void *);
extern void FUN_08a9c304(void *, int *);
extern void FUN_08921160(void *, void *);
extern void FUN_08921170(void *, void *);

void FUN_0892160c(Object *param_1)
{
    int local[2];
    int i;

    FUN_0894a258(param_1);
    param_1->vtable = DAT_08bb6a78;
    param_1->references = 1;
    FUN_0894a258(&param_1->subobject_vtable);
    param_1->vtable = DAT_08bb6a40;
    param_1->subobject_vtable = DAT_08bb6a54;
    local[0] = 0;
    local[1] = 0;
    FUN_08a9c304((char *)param_1 + 16, local);
    param_1->field28 = 0;
    param_1->field2c = 0;
    param_1->field30 = 0;
    param_1->field34 = 0;
    param_1->field38 = 0;
    param_1->field3c = 0;
    param_1->field40 = 0;
    param_1->field44 = 0;
    param_1->field90 = 0;
    for (i = 0; i < 3; ++i) {
        param_1->callbacks48[i] = FUN_08921160;
        param_1->callbacks54[i] = FUN_08921170;
        param_1->callbacks60[i] = 0;
        param_1->callbacks6c[i] = 0;
        param_1->callbacks78[i] = 0;
        param_1->callbacks84[i] = 2;
    }
    param_1->field3c = 0x610;
}
