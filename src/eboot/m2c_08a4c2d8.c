/* LBPMATCH module=EBOOT.BIN start=0x08a4c2d8 end=0x08a4c318 symbol=FUN_08a4c2d8 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_08a9df6c(int);
extern int DAT_08bc5cd0;
extern int DAT_08bc5cf0;

void FUN_08a4c2d8(void *arg0) {
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bc5cd0;
    FUN_08a9df6c((*(int *)((char *)(arg0) + 0x24)));
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bc5cf0;
}
