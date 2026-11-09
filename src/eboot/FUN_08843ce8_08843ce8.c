/* LBPMATCH module=EBOOT.BIN start=0x08843ce8 end=0x08843dc4 symbol=FUN_08843ce8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17g, Codex GPT-6.1); exact match verified; names and types await human review. */

typedef unsigned int uint;
typedef unsigned char byte;

extern char DAT_08bedb94[];
extern char DAT_08bae3fc;
extern char DAT_08bae434;
extern void FUN_08843b10(int *, int *);
extern void FUN_08b9cef4(void *, void *);
extern void FUN_08b9ce78(int, void *);
extern void FUN_08b9ce3c(void *, void *, void *);
extern void FUN_08a9e0c4(void *);

void FUN_08843ce8(char *param_1, void *param_2, int param_3) {
    if (*(byte *)(param_1 + 0x2c) != 0) {
        int iVar2 = 0;
        FUN_08b9ce78(10, param_2);
        while (iVar2 < param_3) {
            ++iVar2;
            FUN_08b9cef4(&DAT_08bae3fc, param_2);
        }
        FUN_08b9ce3c(param_2, &DAT_08bae434, *(char **)(param_1 + 0x20) + 8);
        return;
    }

    uint *local_30[4];
    local_30[0] = (uint *)DAT_08bedb94;
    FUN_08843b10((int *)(param_1 + 0x20), (int *)local_30);
    FUN_08b9cef4(local_30[0] + 2, param_2);
    if (local_30[0] != (uint *)DAT_08bedb94 && local_30[0] != 0) {
        FUN_08a9e0c4(local_30[0]);
    }
}

