/* LBPMATCH module=EBOOT.BIN start=0x089baad0 end=0x089bab10 symbol=FUN_089baad0 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_08974824();
int FUN_08b9eb88(void *, int, int);
extern int DAT_08bc0e28;

void FUN_089baad0(void *arg0) {
    FUN_08974824();
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bc0e28;
    (*(int *)((char *)(arg0) + 0x1C)) = 0;
    FUN_08b9eb88(arg0 + 0x20, 0, 0x40);
}
