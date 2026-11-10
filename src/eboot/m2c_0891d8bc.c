/* LBPMATCH module=EBOOT.BIN start=0x0891d8bc end=0x0891d8e4 symbol=FUN_0891d8bc lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m2); names and types await human review. */

int FUN_0891d8bc(void *arg0) {
    void **temp_v1;

    temp_v1 = (*(void ***)((char *)(arg0) + 0xB4));
    if (temp_v1 != 0) {
        return (*(int (**)(void **))((char *)(*temp_v1) + 0xC))(temp_v1);
    }
    return 0;
}
