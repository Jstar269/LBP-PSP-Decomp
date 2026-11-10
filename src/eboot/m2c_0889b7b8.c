/* LBPMATCH module=EBOOT.BIN start=0x0889b7b8 end=0x0889b800 symbol=FUN_0889b7b8 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_0889ada8(int, int **);
extern int DAT_08bb14d8;
extern int DAT_08bb17a8;
extern int DAT_08bee224;

void FUN_0889b7b8(int **arg0) {
    *arg0 = &DAT_08bb17a8;
    FUN_0889ada8(DAT_08bee224, arg0);
    *arg0 = &DAT_08bb14d8;
}
