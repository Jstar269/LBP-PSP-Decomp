/* LBPMATCH module=EBOOT.BIN start=0x08a58a30 end=0x08a58a58 symbol=FUN_08a58a30 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m4); names and types await human review. */

int FUN_08a58a30(void *arg0) {
    void **temp_v1;

    temp_v1 = (*(void ***)((char *)(arg0) + 0x9C));
    if (temp_v1 != 0) {
        return (*(int (**)(void **))((char *)(*temp_v1) + 0xC))(temp_v1);
    }
    return 0;
}
