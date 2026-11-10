/* LBPMATCH module=EBOOT.BIN start=0x0889bde4 end=0x0889be08 symbol=FUN_0889bde4 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m2); names and types await human review. */

extern int DAT_08bb18e8;
extern void *DAT_08bee258;

void FUN_0889bde4(void *arg0) {
    DAT_08bee258 = arg0;
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bb18e8;
    (*(int *)((char *)(arg0) + 8)) = -1;
    (*(int *)((char *)(arg0) + 4)) = 0;
}
