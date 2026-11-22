/* LBPMATCH module=EBOOT.BIN start=0x088a1170 end=0x088a1210 symbol=FUN_088a1170 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17c, Codex GPT-6); exact match verified; names and types await human review. */
struct Context { int words[75]; };
extern void FUN_08aee0ec(struct Context *, void *);
extern void FUN_08baa50c(void *);
extern void FUN_08baa524(void *, void *);
extern unsigned int FUN_08adbd60(const char *);
extern int FUN_08aee3a8(struct Context *);
extern void FUN_0889be7c(void *, int, char *, char *);
extern void FUN_0889c064(void *);
extern int DAT_08bee3b4;
extern char DAT_08bb2668[];

void FUN_088a1170(void *param_1)
{
    char prefix[6];
    char name[34];
    struct Context context;

    FUN_08aee0ec(&context, 0);
    FUN_08baa50c(prefix);
    FUN_08baa524(prefix, name);
    context.words[0] = 4;
    context.words[6] = DAT_08bee3b4;
    context.words[13] = FUN_08adbd60(name);
    FUN_08aee3a8(&context);
    FUN_0889be7c(param_1, 0, DAT_08bb2668, 0);
    FUN_0889c064(param_1);
}



