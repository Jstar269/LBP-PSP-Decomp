/* LBPMATCH module=EBOOT.BIN start=0x08811e14 end=0x08811eb0 symbol=FUN_08811e14 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp16a, GPT-6); exact match verified; names and types await human review. */
typedef unsigned int uint;
extern void *FUN_08a43fe0(void);
extern int FUN_08a43fec(void *, void *);
extern void FUN_08a49e54(void *);
extern void FUN_08811d24(void *);
extern void FUN_08811f34(void *);
extern void FUN_08811eb0(void *);
void FUN_08811e14(void *param_1)
{
    void *value = FUN_08a43fe0();
    if (FUN_08a43fec(value, param_1) != 0)
        FUN_08a49e54(param_1);
    if (((int (*)(void *))(*(void ***)param_1)[44])(param_1) != 0) {
        FUN_08811d24(param_1);
        return;
    }
    if (*(uint *)((char *)param_1 + 0x44) == 35) {
        FUN_08811eb0(param_1);
        return;
    }
    FUN_08811f34(param_1);
    ((void (*)(void *, int))(*(void ***)param_1)[42])(param_1, 0);
}
