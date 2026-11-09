/* LBPMATCH module=EBOOT.BIN start=0x0887f59c end=0x0887f644 symbol=FUN_0887f59c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17b, Codex GPT-6): matching idiom: weak optional function-address guard preserves the retail branch before each virtual call; exact match verified; names and types await human review. */

typedef void (*Method)(void *);
typedef struct { Method *vtable; } Subobject;
extern void FUN_0887eef8(void *) __attribute__((weak));

void FUN_0887f59c(void *param_1)
{
    void **first = (void **)((char *)param_1 + 0x90);
    void **second = (void **)((char *)param_1 + 0x598);
    *(unsigned int *)((char *)param_1 + 0x2c) = 1;
    int count = 0;
    do {
        void *item = *first;
        ++count;
        first += 12;
        if (item != 0 && FUN_0887eef8 != 0) {
            Subobject *subobject = (Subobject *)((char *)item + 12);
            subobject->vtable[3](subobject);
        }
        item = *second;
        second += 13;
        if (item != 0 && FUN_0887eef8 != 0) {
            Subobject *subobject = (Subobject *)((char *)item + 12);
            subobject->vtable[3](subobject);
        }
    } while (count != 23);
}
