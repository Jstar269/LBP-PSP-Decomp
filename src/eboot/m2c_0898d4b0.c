/* LBPMATCH module=EBOOT.BIN start=0x0898d4b0 end=0x0898d4f4 symbol=FUN_0898d4b0 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_088a827c(int);
int FUN_0897489c(void *);
extern int DAT_08bbfb28;

void FUN_0898d4b0(void *arg0) {
    int temp_v1;

    temp_v1 = (*(int *)((char *)(arg0) + 0x1C));
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bbfb28;
    if (temp_v1 != 0) {
        FUN_088a827c(temp_v1);
    }
    FUN_0897489c(arg0);
}
