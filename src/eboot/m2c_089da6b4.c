/* LBPMATCH module=EBOOT.BIN start=0x089da6b4 end=0x089da6f8 symbol=FUN_089da6b4 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_089da51c();
int FUN_089e01d4(int, int);

void FUN_089da6b4(int arg0, int arg1) {
    if (arg1 & 0x20) {
        FUN_089da51c();
    }
    FUN_089e01d4(arg0, arg1);
}
