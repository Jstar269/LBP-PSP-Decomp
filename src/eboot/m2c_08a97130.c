/* LBPMATCH module=EBOOT.BIN start=0x08a97130 end=0x08a97174 symbol=FUN_08a97130 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_08a0a8ec(void *);
int FUN_08a9e0cc(int);
extern int DAT_08bca300;
extern int DAT_08bca608;

void FUN_08a97130(void *arg0) {
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bca608;
    FUN_08a9e0cc((*(int *)((char *)(arg0) + 0xC8)));
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bca300;
    FUN_08a0a8ec(arg0);
}
