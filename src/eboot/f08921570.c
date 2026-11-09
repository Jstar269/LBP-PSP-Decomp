/* LBPMATCH module=EBOOT.BIN start=0x08921570 end=0x0892160c symbol=FUN_08921570 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18c, GPT-6); exact match verified; names and types await human review. */

typedef struct Object {
    char *vtable;
    int pad04;
    char *subobject_vtable;
    int pad0c;
    unsigned char pad10[0x80];
    void *field90;
} Object;

extern char DAT_08bb6a40[];
extern char DAT_08bb6a54[];
extern char DAT_08bb6a68[];
extern char DAT_08bb6a78[];
extern void FUN_0892145c(Object *);
extern void FUN_088050b8(void *);
extern void FUN_08a9bff8(void *);
extern void FUN_0894a260(void *);
extern void FUN_08a9e0cc(void *);

void FUN_08921570(Object *param_1)
{
    void *old;

    param_1->vtable = DAT_08bb6a40;
    param_1->subobject_vtable = DAT_08bb6a54;
    FUN_0892145c(param_1);
    old = param_1->field90;
    param_1->field90 = 0;
    if (old != 0) {
        FUN_088050b8(old);
    }
    old = param_1->field90;
    if (old != 0) {
        FUN_088050b8(old);
    }
    FUN_08a9bff8((char *)param_1 + 0x10);
    param_1->subobject_vtable = DAT_08bb6a68;
    FUN_0894a260((char *)param_1 + 8);
    param_1->vtable = DAT_08bb6a78;
    FUN_0894a260(param_1);
    FUN_08a9e0cc(param_1);
}
