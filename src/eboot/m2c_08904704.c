/* LBPMATCH module=EBOOT.BIN start=0x08904704 end=0x08904748 symbol=FUN_08904704 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_088050b8(int);
int FUN_0899e45c(void *);
extern int DAT_08bb5b90;

void FUN_08904704(void *arg0) {
    int temp_v1;

    temp_v1 = (*(int *)((char *)(arg0) + 0x2C));
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bb5b90;
    if (temp_v1 != 0) {
        FUN_088050b8(temp_v1);
    }
    FUN_0899e45c(arg0);
}
