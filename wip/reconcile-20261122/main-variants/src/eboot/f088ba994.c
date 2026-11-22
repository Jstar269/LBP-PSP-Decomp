/* LBPMATCH module=EBOOT.BIN start=0x088ba994 end=0x088baa24 symbol=FUN_088ba994 lang=c99 */
/* Hand-written reconstruction from disassembly (lane lbp17d, Codex GPT-6.1): retained pointer update and release call; exact match verified; names and types await human review. */
extern void FUN_08ad3088(int *, int **, int);
extern void FUN_08ad2810(int *);
extern void FUN_088050b8(void *);

void FUN_088ba994(int *param_1, int **param_2, int param_3)
{
    int *value;
    int *old;

    if (param_3 != 3) {
        return;
    }

    value = ((int *)*param_2) + 10;
    value = (int *)*value;
    if (value != 0) {
        FUN_08ad3088(value + 5, param_2, 3);
        value[2] = value[2] + 1;
        FUN_08ad2810(value + 5);
    }

    old = (int *)param_1[15];
    param_1[15] = (int)value;
    if (old != 0) {
        FUN_088050b8(old);
    }
}
