/* LBPMATCH module=EBOOT.BIN start=0x089185a4 end=0x08918654 symbol=FUN_089185a4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18b, GPT-6); exact match verified; names and types await human review. */
typedef void (*SetupFn)(void *, int, void *, int, int);
extern char DAT_08bef8f4[];
extern int FUN_08918580(void);
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_088050b8(void *);
void FUN_089185a4(void *self, int **param_2)
{
    int *shared = *param_2;
    void *target = *(void **)((char *)shared + 0xa8);
    void **vtable = *(void ***)target;
    ((SetupFn)vtable[7])(target, (int)FUN_08918580, self, 0, 1);
    shared = *param_2;
    if (shared != 0) {
        FUN_08ad3088((char *)shared + 0x14);
        shared[2] = shared[2] + 1;
        FUN_08ad2810((char *)shared + 0x14);
    }
    int *old = *(int **)((char *)self + 0x54);
    *(int **)((char *)self + 0x54) = shared;
    if (old != 0)
        FUN_088050b8(old);
}
