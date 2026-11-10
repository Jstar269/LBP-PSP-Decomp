/* LBPMATCH module=EBOOT.BIN start=0x0889bbe4 end=0x0889bc0c symbol=FUN_0889bbe4 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m2); names and types await human review. */

extern void **DAT_08bee258;

void FUN_0889bbe4(void) {
    if (DAT_08bee258 != 0) {
        (*(int (**)(void **))((char *)(*DAT_08bee258) + 4))(DAT_08bee258);
    }
}
