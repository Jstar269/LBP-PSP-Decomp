/* LBPMATCH module=EBOOT.BIN start=0x08937564 end=0x08937614 symbol=FUN_08937564 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18d, Codex GPT-6): exact match verified; names and types await human review. */
typedef struct Obj Obj;
typedef struct { Obj *p; } Ref;
struct Obj { int vtable; int references; int pad[13]; unsigned short flags; };
extern void FUN_08931ce0(int *, int);
extern void FUN_089374b4(void *, Ref *);

void FUN_08937564(char *param_1, Ref *param_2)
{
    Obj *object = param_2->p;
    unsigned short flags = object->flags;
    if ((flags & 0x10) != 0) {
        if ((flags & 4) != 0) {
            FUN_089374b4(param_1 + 420, param_2);
            return;
        }
        FUN_089374b4(param_1 + 412, param_2);
        return;
    }
    if ((flags & 2) != 0) {
        int *cursor = (int *)object;
        int index = 0;
        do {
            if (cursor[27] != 0) {
                FUN_08931ce0((int *)object, index);
            }
            cursor++;
            index++;
        } while (index != 3);
        FUN_089374b4(param_1 + 628, param_2);
        return;
    }
    FUN_089374b4(param_1 + 388, param_2);
}
