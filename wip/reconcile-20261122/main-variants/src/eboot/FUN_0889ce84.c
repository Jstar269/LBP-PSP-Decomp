/* LBPMATCH module=EBOOT.BIN start=0x0889ce84 end=0x0889cf24 symbol=FUN_0889ce84 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17c, Codex GPT-6); exact match verified; names and types await human review. */
struct Context {
    void *base;
    unsigned char option;
    unsigned char padding[3];
    int mode;
    int data[11];
    int selection;
    int padding2[3];
    int source;
};
extern void *FUN_08aee084(void);
extern int FUN_08b2f318(struct Context *);
extern int FUN_08b31d10(void *, int *);
extern void FUN_08b2f364(struct Context *, int *);

void FUN_0889ce84(int param_1, int param_2, int param_3, unsigned char param_4)
{
    int local_a0[12];
    struct Context context;
    void *value = FUN_08aee084();

    FUN_08b2f318(&context);
    context.base = value;
    context.option = param_4;
    context.mode = param_3;
    context.selection = param_2;
    context.source = param_1;
    FUN_08b31d10(value, context.data);
    context.data[0] = -1;
    local_a0[0] = 0;
    FUN_08b2f364(&context, local_a0);
}

