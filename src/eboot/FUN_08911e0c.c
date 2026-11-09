/* LBPMATCH module=EBOOT.BIN start=0x08911e0c end=0x08911ec8 symbol=FUN_08911e0c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18b, GPT-6); exact match verified; matching idiom: volatile reset stores preserve the observed teardown field order; names and types await human review. */
typedef unsigned int uint;
extern int FUN_08a50fbc(void);
extern int *FUN_08a52bb8(int, uint *);
extern void FUN_08a9d390(uint *, int);
extern void FUN_08a9bff8(void *);
void FUN_08911e0c(unsigned char *param_1)
{
    uint second[6];
    uint first[6];
    int *object;
    int *vtable;
    int manager;
    if (*param_1 != 0) {
        FUN_08a50fbc();
        object = *(int **)(param_1 + 0xc);
        if (object != 0) {
            vtable = *(int **)object;
            ((void (*)(int *))vtable[1])(object);
        }
        *(int *)(param_1 + 0xc) = 0;
        manager = FUN_08a50fbc();
        object = FUN_08a52bb8(manager, (uint *)(param_1 + 8));
        if (object != 0) {
            vtable = *(int **)object;
            ((void (*)(int *))vtable[0x128 / 4])(object);
        }
        FUN_08a9d390(first, -1);
        FUN_08a9bff8(first);
        FUN_08a9d390(second, -1);
        FUN_08a9bff8(second);
        *(volatile int *)(param_1 + 8) = -1;
        *(volatile int *)(param_1 + 16) = 0;
        *(volatile unsigned char *)param_1 = 0;
    }
}



