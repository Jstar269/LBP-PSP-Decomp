/* LBPMATCH module=EBOOT.BIN start=0x0899fd80 end=0x0899fda8 symbol=FUN_0899fd80 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m2); names and types await human review. */

int FUN_0899fd80(void *arg0) {
    void **temp_v1;

    temp_v1 = (*(void ***)((char *)(arg0) + 0x30));
    if (temp_v1 != 0) {
        return (*(int (**)(void **))((char *)(*temp_v1) + 0x38))(temp_v1);
    }
    return 0;
}
