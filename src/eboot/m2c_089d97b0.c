/* LBPMATCH module=EBOOT.BIN start=0x089d97b0 end=0x089d97f0 symbol=FUN_089d97b0 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_0884224c(int, int);
extern int DAT_08bedb54;

int FUN_089d97b0(void *arg0) {
    int temp_v1;
    int var_v1;

    temp_v1 = (*(int *)((char *)(arg0) + 0x2C));
    if ((temp_v1 == -1) || (var_v1 = 0, (FUN_0884224c(DAT_08bedb54, temp_v1) != 0))) {
        var_v1 = 1;
    }
    return var_v1;
}
