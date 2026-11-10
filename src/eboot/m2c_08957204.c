/* LBPMATCH module=EBOOT.BIN start=0x08957204 end=0x08957234 symbol=FUN_08957204 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m4); names and types await human review. */

int FUN_08b9eb88(void *, int, int);
extern int DAT_08bb7d58;

void FUN_08957204(void *arg0) {
    (*(int *)((char *)(arg0) + 4)) = 1;
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bb7d58;
    (*(int *)((char *)(arg0) + 0x1C)) = 0x20;
    FUN_08b9eb88(arg0 + 8, 0, 0x14);
}
