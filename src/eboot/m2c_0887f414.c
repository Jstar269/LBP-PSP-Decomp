/* LBPMATCH module=EBOOT.BIN start=0x0887f414 end=0x0887f45c symbol=FUN_0887f414 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_0887f414(int arg0, int arg1) {
    int var_v0;
    void *temp_v1;

    temp_v1 = (*(void **)((char *)(((arg1 * 0x30) + arg0)) + 0x90));
    var_v0 = 0;
    if (temp_v1 != 0) {
        (*(int (**)(int, int))((char *)((*(void **)((char *)(temp_v1) + 0xC))) + 0xC))(temp_v1 + 0xC, arg1 * 0x10);
        var_v0 = 1;
    }
    return var_v0;
}
