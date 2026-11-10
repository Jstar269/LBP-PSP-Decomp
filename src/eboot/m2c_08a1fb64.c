/* LBPMATCH module=EBOOT.BIN start=0x08a1fb64 end=0x08a1fb94 symbol=FUN_08a1fb64 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m4); names and types await human review. */

void FUN_08a1fb64(void *arg0) {
    int var_v1;
    void *var_v0;

    (*(int *)((char *)(arg0) + 0)) = 0;
    var_v0 = arg0;
    var_v1 = 0;
    (*(int *)((char *)(arg0) + 4)) = 0;
    do {
        var_v1 += 1;
        (*(int *)((char *)(var_v0) + 8)) = 0;
        (*(int *)((char *)(var_v0) + 0xC)) = 0;
        var_v0 += 8;
    } while (var_v1 != 0x11);
}
