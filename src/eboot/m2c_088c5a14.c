/* LBPMATCH module=EBOOT.BIN start=0x088c5a14 end=0x088c5a3c symbol=FUN_088c5a14 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m2); names and types await human review. */

extern void **DAT_08beee0c;

void FUN_088c5a14(void) {
    if (DAT_08beee0c != 0) {
        (*(int (**)(void **))((char *)(*DAT_08beee0c) + 0x28))(DAT_08beee0c);
    }
}
