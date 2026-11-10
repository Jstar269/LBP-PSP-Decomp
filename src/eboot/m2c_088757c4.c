/* LBPMATCH module=EBOOT.BIN start=0x088757c4 end=0x088757ec symbol=FUN_088757c4 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m2); names and types await human review. */

int FUN_088757c4(void *arg0) {
    int temp_v1;
    int var_a1;

    temp_v1 = (*(int *)((char *)(arg0) + 0x40));
    var_a1 = temp_v1;
    if (temp_v1 == -1) {
        var_a1 = 0;
        if ((*(unsigned char *)((char *)(arg0) + 0x44)) == 0) {
            var_a1 = (*(int *)((char *)(arg0) + 0x38));
        }
    }
    return var_a1;
}
