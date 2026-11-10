/* LBPMATCH module=EBOOT.BIN start=0x089da834 end=0x089da858 symbol=FUN_089da834 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m2); names and types await human review. */

int FUN_089da834(void *arg0) {
    int temp_v0;
    int var_v1;
    void *temp_a0;

    temp_v0 = (*(int *)((char *)(arg0) + 0x20));
    var_v1 = temp_v0;
    if (temp_v0 == 0) {
        temp_a0 = (*(void **)((char *)(arg0) + 0x44));
        var_v1 = 0;
        if (temp_a0 != 0) {
            var_v1 = (*(int *)((char *)(temp_a0) + 0xA8));
        }
    }
    return var_v1;
}
