/* LBPMATCH module=EBOOT.BIN start=0x088befd4 end=0x088beff0 symbol=FUN_088befd4 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m2); names and types await human review. */

extern void *DAT_08bf03b8;

void FUN_088befd4(void *arg0) {
    void **temp_a0;

    temp_a0 = (*(void ***)((char *)(arg0) + 0x30));
    (*(int (**)(void **, int))((char *)(*temp_a0) + 0xC))(temp_a0, (*(int *)((char *)(DAT_08bf03b8) + 0x84)));
}
