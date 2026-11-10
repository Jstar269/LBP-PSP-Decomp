/* LBPMATCH module=EBOOT.BIN start=0x0885cc5c end=0x0885cca4 symbol=FUN_0885cc5c lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_0887c648(int, int);
int FUN_0887e0ac();
extern int DAT_08bedba4;

int FUN_0885cc5c(void *arg0) {
    int temp_v0;

    temp_v0 = FUN_0887e0ac();
    if (temp_v0 != 0) {
        FUN_0887c648(DAT_08bedba4, (*(int *)((char *)(arg0) + 0xC)));
    }
    return temp_v0;
}
