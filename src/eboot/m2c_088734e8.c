/* LBPMATCH module=EBOOT.BIN start=0x088734e8 end=0x08873510 symbol=FUN_088734e8 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m2); names and types await human review. */

int FUN_0886c704();

int FUN_088734e8(void *arg0) {
    int temp_v0;
    int temp_v0_2;
    int var_v1;

    temp_v0 = (*(int *)((char *)(arg0) + 0x3C));
    var_v1 = temp_v0;
    if (temp_v0 == 0) {
        temp_v0_2 = (*(int *)((char *)(arg0) + 0x38));
        var_v1 = temp_v0_2;
        if (temp_v0_2 == 0) {
            return FUN_0886c704();
        }
    }
    return var_v1;
}
