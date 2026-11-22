/* LBPMATCH module=EBOOT.BIN start=0x0893773c end=0x089377f0 symbol=FUN_0893773c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18d, Codex GPT-6): exact match verified; names and types await human review. */
typedef struct Obj Obj;
typedef struct { Obj *p; } Ref;
struct Obj {
    unsigned char pad[52];
    int field34;
    int field38;
    unsigned short flags;
};

extern unsigned char *DAT_08bedb1c;
extern unsigned char *DAT_08bedb14;
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_089374b4(void *, Ref *);

int FUN_0893773c(Ref *param_1)
{
    unsigned char *state = DAT_08bedb1c;
    void *lock = state + 1556;
    FUN_08ad3088(lock);
    unsigned char active = *(state + 1780);
    FUN_08ad2810(lock);

    if (active != 0 || (param_1->p->flags & 2) != 0) {
        unsigned char *context = DAT_08bedb14;
        lock = context + 100;
        FUN_08ad3088(lock);
        Obj *object = param_1->p;
        object->field34 = 0;
        object->flags = object->flags | 0x40;
        object->field38 = 0;
        FUN_089374b4(context + 364, param_1);
        FUN_08ad2810(lock);
        return 1;
    }
    return 0;
}
