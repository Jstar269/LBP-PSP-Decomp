/* LBPMATCH module=EBOOT.BIN start=0x0887f45c end=0x0887f4fc symbol=FUN_0887f45c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17b, Codex GPT-6); exact match verified; names and types await human review. */

typedef struct Object Object;
typedef void (*Callback)(void *, void *);
typedef void (*RegisterCallback)(Object *, Callback, void *);
struct Object { RegisterCallback *vtable; };

extern Object *DAT_08bedd04;
extern Object *DAT_08bedc38;
extern Object *DAT_08bedc30;
extern Object *DAT_08bedc34;
extern void FUN_0887f1b4(void *);
extern void FUN_0887f1bc(void *);
extern void FUN_0887f1c4(void *);
extern void FUN_0887f1cc(void *);

void FUN_0887f45c(void *param_1)
{
    Object *object = DAT_08bedd04;
    object->vtable[2](object, (Callback)FUN_0887f1b4, param_1);
    object = DAT_08bedc38;
    object->vtable[2](object, (Callback)FUN_0887f1bc, param_1);
    object = DAT_08bedc30;
    object->vtable[2](object, (Callback)FUN_0887f1c4, param_1);
    object = DAT_08bedc34;
    object->vtable[2](object, (Callback)FUN_0887f1cc, param_1);
}
