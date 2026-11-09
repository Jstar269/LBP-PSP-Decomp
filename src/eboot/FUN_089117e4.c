/* LBPMATCH module=EBOOT.BIN start=0x089117e4 end=0x089118bc symbol=FUN_089117e4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18b, GPT-6); exact match verified; names and types await human review. */
typedef unsigned int uint;
struct Output { uint ready; uint value; };
extern char *DAT_08bf7304;
extern int DAT_08bef404;
extern void FUN_08a50fbc(void);
extern void FUN_08910574(void *, int);
extern int *FUN_08914a7c(void);
extern int FUN_08914e94(int *, int);
extern int FUN_08913dd8(void *, int, int, int, int, int, int, int);
void FUN_089117e4(int *param_1)
{
    int *first;
    int *second;
    int value;
    int result;
    FUN_08a50fbc();
    FUN_08910574(DAT_08bf7304 + 0x87c, 3);
    first = FUN_08914a7c();
    second = FUN_08914a7c();
    value = FUN_08914e94(second, DAT_08bef404);
    FUN_08913dd8(first, 0, value, 2, 0, 0, 0x400, 0);
    first = FUN_08914a7c();
    second = FUN_08914a7c();
    value = FUN_08914e94(second, DAT_08bef404);
    result = FUN_08913dd8(first, 0, value, 4, 0, 0, 0x400, 0);
    *(struct Output *)(param_1 + 6) = (struct Output){1, (uint)result};
}
