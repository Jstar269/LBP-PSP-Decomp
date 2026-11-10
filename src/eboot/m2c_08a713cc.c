/* LBPMATCH module=EBOOT.BIN start=0x08a713cc end=0x08a713fc symbol=FUN_08a713cc lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m4); names and types await human review. */

int FUN_08a713cc(void *arg0) {
    int *var_v1;
    int var_a1;
    int var_v0;
    void *temp_a0;
    void *temp_v0;

    temp_v0 = (*(void **)((char *)(arg0) + 0xA4));
    var_v1 = (*(int **)((char *)(temp_v0) + 0x54));
    temp_a0 = temp_v0 + 0x54;
    var_a1 = 0;
    if (var_v1 != temp_a0) {
        var_v0 = 0;
        do {
            var_v1 = *var_v1;
            var_v0 += 1;
        } while (temp_a0 != var_v1);
        var_a1 = var_v0;
    }
    return var_a1;
}
