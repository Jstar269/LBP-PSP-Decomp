/* LBPMATCH module=EBOOT.BIN start=0x089e01d4 end=0x089e0214 symbol=FUN_089e01d4 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_089dfec0();

void FUN_089e01d4(void *arg0) {
    void **temp_v0;

    if ((*(int *)((char *)(arg0) + 0x40)) != 0) {
        FUN_089dfec0();
    }
    temp_v0 = (*(void ***)((char *)(arg0) + 0x34));
    (*(int (**)(void **))((char *)(*temp_v0) + 0x24))(temp_v0);
}
