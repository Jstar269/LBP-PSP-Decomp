/* LBPMATCH module=EBOOT.BIN start=0x08849ff0 end=0x0884a01c symbol=FUN_08849ff0 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m4); names and types await human review. */

int FUN_08849ff0(int arg0) {
    int var_a1;
    void *temp_a0;

    temp_a0 = arg0 + 0xC;
    var_a1 = 1;
    if (((*(unsigned char *)((char *)(temp_a0) + 8)) != 0) || ((*(int *)((char *)(temp_a0) + 4)) != 3)) {
        var_a1 = 0;
    }
    return var_a1;
}
