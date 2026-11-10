/* LBPMATCH module=EBOOT.BIN start=0x08b989d4 end=0x08b989f4 symbol=FUN_08b989d4 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m2); names and types await human review. */

void FUN_08b989d4(void *arg0) {
    int (*temp_t9)(int, void *);

    temp_t9 = (*(int (**)(int, void *))((char *)(arg0) + 8));
    if (temp_t9 != 0) {
        temp_t9(1, arg0);
    }
}
