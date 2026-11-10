/* LBPMATCH module=EBOOT.BIN start=0x08843570 end=0x088435b8 symbol=FUN_08843570 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_08843570(void *arg0) {
    void **temp_v0_2;
    void *temp_v0;

    temp_v0_2 = (*(void ***)((char *)(arg0) + 0x18));
    if (temp_v0_2 != 0) {
        temp_v0 = (*(void *(**)(void **))((char *)(*temp_v0_2) + 0x20))(temp_v0_2);
        if (temp_v0 != 0) {
            return (*(int *)((char *)(temp_v0) + 0x20)) + 8;
        }
    }
    return 0;
}
