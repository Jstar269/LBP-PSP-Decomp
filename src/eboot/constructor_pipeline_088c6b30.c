/* LBPMATCH module=EBOOT.BIN start=0x088c6b30 end=0x088c6bd0 symbol=FUN_088c6b30 lang=c99 */
/* Hand-written reconstruction from disassembly (lane lbp17d, Codex GPT-6.1): builds the two objects, dispatches the virtual callbacks, and tears down the local context; exact match verified; names and types await human review. */
extern void FUN_089463d4(void *);
extern void *FUN_08a9e838(int);
extern void FUN_088bf55c(void *);
extern void FUN_088c2380(void *, void *, int);
extern void FUN_0883bd5c(int *, int);
extern void FUN_088b2aa4(int *);
extern void FUN_08804f3c(int *);
extern int DAT_08bedb1c;
typedef int (*GetValue)(void *);
typedef void (*SetValue)(void *, int);

void FUN_088c6b30(void)
{
    FUN_089463d4((void *)DAT_08bedb1c);
    void *first = FUN_08a9e838(0x34);
    FUN_088bf55c(first);
    void *second = FUN_08a9e838(0x94);
    FUN_088c2380(second, first, 9);
    SetValue set = (SetValue)(*(void ***)second)[19];
    int result = ((GetValue)(*(void ***)first)[16])(first);
    set(second, result);
    int stack[4];
    FUN_0883bd5c(stack, (int)second);
    FUN_088b2aa4(stack);
    FUN_08804f3c(stack);
}
