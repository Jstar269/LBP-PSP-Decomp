/* LBPMATCH module=EBOOT.BIN start=0x08a08e40 end=0x08a08e5c symbol=FUN_08a08e40 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m2); names and types await human review. */

extern int DAT_08bc2fc0;

void FUN_08a08e40(void *arg0, int arg1, int arg2) {
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bc2fc0;
    (*(int *)((char *)(arg0) + 4)) = arg1;
    (*(int *)((char *)(arg0) + 8)) = arg2;
    (*(int *)((char *)(arg0) + 0xC)) = 0;
}
