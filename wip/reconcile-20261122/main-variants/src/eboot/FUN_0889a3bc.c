/* LBPMATCH module=EBOOT.BIN start=0x0889a3bc end=0x0889a450 symbol=FUN_0889a3bc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17c, Codex GPT-6); exact match verified; names and types await human review. */
struct Obj { unsigned char bytes[20]; };
extern struct Obj *DAT_08bee22c;
extern void **DAT_08bee228;
extern unsigned char DAT_08bb14a0[];
extern unsigned char DAT_08bb1518[];
extern struct Obj *FUN_08a9e838(int);
extern void *FUN_08baa24c(unsigned char *, int, int, int);

struct Obj *FUN_0889a3bc(void)
{
    if (DAT_08bee22c == 0) {
        struct Obj *obj = FUN_08a9e838(20);
        *(unsigned char **)obj = DAT_08bb14a0;
        obj->bytes[8] = 0;
        obj->bytes[9] = 0;
        obj->bytes[10] = 0;
        obj->bytes[11] = 0;
        obj->bytes[16] = 0;
        DAT_08bee22c = obj;
        void **slot = (void **)FUN_08a9e838(4);
        void *value = FUN_08baa24c(DAT_08bb1518, 0, 0, 0);
        DAT_08bee228 = slot;
        *slot = value;
    }
    return DAT_08bee22c;
}

