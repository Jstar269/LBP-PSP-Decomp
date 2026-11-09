/* LBPMATCH module=EBOOT.BIN start=0x08843e64 end=0x08843f04 symbol=FUN_08843e64 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp16b, GPT-6); exact match verified; names and types await human review. */
struct Buf { unsigned int len; unsigned int cap; char data[1]; };
struct Str { struct Buf *buf; };
typedef void (*Callback)(void *, int, int, struct Str *);

extern struct Str *FUN_088430b0(struct Str *, int *, int);

int FUN_08843e64(char *param_1, void *param_2)
{
    int i = 0;
    struct Str *str;

    while (i < *(int *)(param_1 + 4)) {
        int *item;

        str = (struct Str *)(param_1 + 12);
        item = *(int **)(param_1 + 16);
        FUN_088430b0(str, item + 2, *item);
        i = i + 1;
    }
    str = (struct Str *)(param_1 + 12);

    (*(Callback **)param_2)[18](param_2, 0, 0, str);
    {
        int *item = *(int **)(param_1 + 20);
        FUN_088430b0(str, item + 2, *item);
    }
    return 1;
}
