/* LBPMATCH module=EBOOT.BIN start=0x08844f48 end=0x08845010 symbol=FUN_08844f48 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp16b, GPT-6); exact match verified; names and types await human review. */
struct Buf { unsigned int len; unsigned int cap; char data[1]; };
struct Str { struct Buf *buf; };
extern struct Str *FUN_088430b0(struct Str *, const void *, int);
extern unsigned int FUN_08ba0e8c(const char *);
extern char DAT_08bae4dc[];
extern char DAT_08bae4e4[];

int FUN_08844f48(char *param_1, char *param_2)
{
    int i = 0;
    struct Str *str;
    char *text;
    unsigned int len;
    int *item;

    while (i < *(int *)(param_1 + 4)) {
        str = (struct Str *)(param_1 + 12);
        item = *(int **)(param_1 + 16);
        FUN_088430b0(str, item + 2, *item);
        i = i + 1;
    }
    str = (struct Str *)(param_1 + 12);
    FUN_088430b0(str, DAT_08bae4dc, 4);
    text = *(char **)(param_2 + 32) + 8;
    len = FUN_08ba0e8c(text);
    FUN_088430b0(str, text, len);
    FUN_088430b0(str, DAT_08bae4e4, 3);
    item = *(int **)(param_1 + 20);
    FUN_088430b0(str, item + 2, *item);
    return 1;
}
