/* LBPMATCH module=EBOOT.BIN start=0x08883308 end=0x088833d8 symbol=FUN_08883308 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17h, GPT-6.1 Terra); exact match verified; names and types await human review. */
typedef struct Ref { int refs; } Ref;
extern Ref *DAT_08bedfb0;
extern void *FUN_08a9e838(unsigned int);
extern void FUN_088817a0(Ref *);
extern void FUN_0888091c(Ref *);
extern void FUN_08adb8a4(void *);
Ref **FUN_08883308(Ref **out) {
    Ref *object = (Ref *)FUN_08a9e838(0xa24);
    FUN_088817a0(object);
    if (object != 0) {
        ++object->refs;
    }
    Ref *old = DAT_08bedfb0;
    DAT_08bedfb0 = object;
    if (old != 0) {
        int refs = --old->refs;
        if (refs == 0) {
            FUN_0888091c(old);
            FUN_08adb8a4(old);
        }
    }
    if (object != 0) {
        int refs = --object->refs;
        if (refs == 0) {
            FUN_0888091c(object);
            FUN_08adb8a4(object);
        }
    }
    Ref *current = DAT_08bedfb0;
    *out = current;
    if (current != 0) {
        ++current->refs;
    }
    return out;
}

