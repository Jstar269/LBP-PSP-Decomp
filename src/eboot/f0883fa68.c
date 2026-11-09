/* LBPMATCH module=EBOOT.BIN start=0x0883fa68 end=0x0883fb18 symbol=FUN_0883fa68 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp16a, GPT-6); exact match verified; names and types await human review. */
typedef unsigned int uint;
typedef unsigned char uchar;
extern unsigned char DAT_08bf6e94;
extern void FUN_0883e504(void *, int);
extern void FUN_0883c3b4(void *);
extern void *FUN_08a9e838(uint);
extern void FUN_089558f8(void *);
extern void *FUN_08a50fbc(void);
extern void FUN_08955a04(void *, uint *, int);
extern void FUN_08956130(void *);
extern void FUN_08a9e0cc(void *);
extern void FUN_088406ac(void *);
void FUN_0883fa68(int *param_1)
{
    void *object;
    int *item;
    FUN_0883e504(param_1, 0);
    FUN_0883c3b4((char *)param_1 + 0xa4);
    DAT_08bf6e94 = 0;
    object = FUN_08a9e838(16);
    FUN_089558f8(object);
    param_1[0x25] = (int)object;
    param_1[0x3c] = (int)FUN_08a50fbc();
    FUN_08955a04((void *)param_1[0x25], (uint *)(param_1 + 0x3d), 0);
    object = (void *)param_1[0x25];
    if (object != 0) {
        FUN_08956130(object);
        FUN_08a9e0cc(object);
    }
    param_1[0x25] = 0;
    for (item = (int *)param_1[0]; item != (int *)param_1[1]; item = (int *)((char *)item + 0x1c))
        FUN_088406ac(item);
    *(uchar *)((char *)param_1 + 0x108) = 0;
}
