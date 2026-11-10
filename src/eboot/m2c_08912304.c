/* LBPMATCH module=EBOOT.BIN start=0x08912304 end=0x08912344 symbol=FUN_08912304 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_08912a38();
extern float DAT_08babcbc;
extern int DAT_08bb61b8;

void FUN_08912304(void *arg0) {
    FUN_08912a38();
    (*(float *)((char *)(arg0) + 0x3C)) = (float) DAT_08babcbc;
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bb61b8;
    (*(int *)((char *)(arg0) + 0x38)) = 0;
}
