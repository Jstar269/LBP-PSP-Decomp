/* LBPMATCH module=EBOOT.BIN start=0x089bb154 end=0x089bb194 symbol=FUN_089bb154 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_089739c0(int);
int FUN_0897489c(void *);
int FUN_08a9e0cc(void *);
extern int DAT_08bc0e70;

void FUN_089bb154(void *arg0) {
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bc0e70;
    FUN_089739c0((*(int *)((char *)(arg0) + 0x14)));
    FUN_0897489c(arg0);
    FUN_08a9e0cc(arg0);
}
