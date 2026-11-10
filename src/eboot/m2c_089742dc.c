/* LBPMATCH module=EBOOT.BIN start=0x089742dc end=0x08974310 symbol=FUN_089742dc lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m4); names and types await human review. */

void FUN_089742dc(void *arg0) {
    void *temp_v1;

    temp_v1 = (*(void **)((char *)(arg0) + 0x14));
    if ((temp_v1 != 0) && ((*(int *)((char *)(temp_v1) + 4)) & 4)) {
        (*(int (**)(void *))((char *)((*(void **)((char *)(temp_v1) + 0))) + 0x1C))(temp_v1);
    }
}
