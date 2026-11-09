/* LBPMATCH module=EBOOT.BIN start=0x088569a4 end=0x08856a5c symbol=FUN_088569a4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp16b, GPT-6); exact match verified; names and types await human review. */
extern char DAT_08baf120[];
extern char DAT_08baf158[];
extern void FUN_088565c4(void);
extern void FUN_08879430(void *, void *, void *, int);
extern void FUN_0885663c(void *);
extern void FUN_08856798(void *, void *, int);
extern void FUN_0886b2c0(void *);
extern void FUN_08856108(void *);

void FUN_088569a4(char *param_1, void *param_2, void *param_3, int param_4, unsigned char param_5)
{
    unsigned int *field;
    int count;

    FUN_08879430(param_1, param_2, FUN_088565c4, 3);
    *(char **)(param_1 + 0) = DAT_08baf120;
    *(char ***)(param_1 + 12) = (char **)DAT_08baf158;
    *(unsigned char *)(param_1 + 0xd0) = param_5;
    *(int *)(param_1 + 0xc8) = 0;
    *(int *)(param_1 + 0xcc) = 0;
    FUN_0885663c(param_1 + 0xd4);
    FUN_08856798(param_1 + 0x1a4, param_3, param_4);
    FUN_0886b2c0(param_1 + 0x28c);
    FUN_08856108(param_1 + 0x35c);
    field = (unsigned int *)(param_1 + 0x364);
    count = 17;
    do {
        count = count - 1;
        *field = 0;
        field = field + 1;
    } while (count != 0);
}
